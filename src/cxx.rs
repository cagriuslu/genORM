use crate::{CxxOptions, Member, ObjectType};
use std::fs;

fn generate_header(output_dir: &str, file_prefix: &str, namespace: &str, object_types: &Vec<ObjectType>) -> Result<(), String> {
    let headers_and_namespace = format!("\
        // Auto-generated file. Changes will be overridden.\n\
        #pragma once\n\
        #include <genORM/genORM.h>\n\
        #include <vector>\n\
        #include <string>\n\
        #include <optional>\n\
        \n\
        namespace {namespace} {{\n\
        ");

    let mut type_declarations = String::new();
    for object_type in object_types {
        let class_name = &object_type.name;
        if object_type.members.is_empty() {
            return Err(format!("Object type has no members: {class_name}"))
        }

        let description = object_type.description.as_ref()
            .map(|s| String::from("    /// ") + &s + "\n")
            .unwrap_or(String::new());

        let open_class = format!("    class {class_name} final : public genORM::object {{\n");

        let validated_members = object_type.members.iter()
            .map(|m| { m.validate() })
            .collect::<Result<Vec<&dyn MemberExt>, String>>()?;
        let member_declarations = validated_members.iter()
            .map(|m| m.format_declaration())
            .collect::<Vec<String>>()
            .join("");

        let member_constructor_parameters = validated_members.iter()
            .map(|m| m.format_constructor_parameter())
            .collect::<Vec<String>>()
            .join(", ");
        let constructor_declaration = format!("        explicit {class_name}(genORM::database& __db, uint64_t __id, {member_constructor_parameters});\n\n");

        let create_declaration = format!("    public:\n        static std::expected<{class_name}, std::string> create(genORM::database& __db, uint64_t __id, {member_constructor_parameters});\n\n");

        let getter_declarations = validated_members.iter()
            .map(|m| m.format_getter_declaration())
            .collect::<Vec<String>>()
            .join("");

        let setter_declarations = validated_members.iter()
            .map(|m| m.format_setter_declaration())
            .collect::<Vec<String>>()
            .join("");

        let close_class = format!("    }};\n");
        type_declarations.push_str(&format!("{description}{open_class}{member_declarations}\n{constructor_declaration}{create_declaration}{getter_declarations}\n{setter_declarations}{close_class}\n"));
    }

    let close_namespace = "}\n";

    fs::write(String::from(output_dir) + file_prefix + ".orm.h",
              format!("{headers_and_namespace}{type_declarations}{close_namespace}"))
        .map_err(|e| e.to_string())?;

    Ok(())
}

fn generate_source(output_dir: &str, file_prefix: &str, namespace: &str, object_types: &Vec<ObjectType>) -> Result<(), String> {
    let include_headers = format!("\
        // Auto-generated file. Changes will be overridden.\n\
        #include \"{file_prefix}.orm.h\"\n\n");

    let mut type_definitions = String::new();
    for object_type in object_types {
        let class_name = &object_type.name;

        let constructor_parameters = object_type.members.iter()
            .map(|m| m.format_constructor_parameter())
            .collect::<Vec<String>>()
            .join(", ");
        let constructor_implementations = object_type.members.iter()
            .map(|m| m.format_constructor_implementation())
            .collect::<Vec<String>>()
            .join(", ");
        let constructor = format!("{namespace}::{class_name}::{class_name}(genORM::database& __db, uint64_t __id, {constructor_parameters})\n    \
            : object(__db, __id), {constructor_implementations} {{}}\n\n");

        let create_table_statement = object_type.members.iter()
            .map(|m| m.format_database_schema())
            .collect::<Vec<String>>()
            .join(", ");
        let insert_statement = vec![String::from("?"); object_type.members.len()].join(", ");

        let binder_implementation = (1..=object_type.members.len())
            .map(|i| {
                let member_impl = object_type.members[i - 1].format_binder_implementation();
                return format!("        if (value_index == {i}) {{ return {member_impl}; }}");
            }).collect::<Vec<_>>().join("\n") + "\n        throw std::logic_error(\"Implementation error\");\n";
        let member_count = object_type.members.len();

        let constructor_call = object_type.members.iter()
            .map(|m| m.format_constructor_call())
            .collect::<Vec<_>>()
            .join(", ");

        let create_implementation = format!("std::expected<{namespace}::{class_name}, std::string> {namespace}::{class_name}::create(genORM::database& __db, {constructor_parameters}) {{\n    \
            static constexpr std::string_view create_table_statement = \"CREATE TABLE IF NOT EXISTS {class_name} (__id INTEGER PRIMARY KEY NOT NULL, {create_table_statement}) STRICT;\";\n    \
            if (auto create_table_result = create_table_if_not_exists(__db, create_table_statement); not create_table_result) {{ return std::unexpected{{std::move(create_table_result.error())}}; }}\n    \
            static constexpr std::string_view insert_statement = \"INSERT INTO {class_name} VALUES (NULL, {insert_statement});\";\n    \
            const auto binder = [&](int value_index) {{\n\
            {binder_implementation}    }};\n    \
            if (auto create_table_result = insert_into_table(__db, insert_statement, {member_count}, binder)) {{\n        \
            return {class_name}{{__db, *create_table_result, {constructor_call}}};\n    \
            }} else {{\n        \
            return std::unexpected{{std::move(create_table_result.error())}};\n    \
            }}\n}}\n\n");

        type_definitions.push_str(&format!("{constructor}{create_implementation}"));
    }

    fs::write(String::from(output_dir) + file_prefix + ".orm.cc",
              format!("{include_headers}{type_definitions}"))
        .map_err(|e| e.to_string())?;

    Ok(())
}

pub fn generate(output_dir: String, cxx_options: &CxxOptions, object_types: &Vec<ObjectType>) -> Result<(), String> {
    generate_header(output_dir.as_str(), &cxx_options.file_prefix, &cxx_options.namespace, object_types)?;
    generate_source(output_dir.as_str(), &cxx_options.file_prefix, &cxx_options.namespace, object_types)?;
    Ok(())
}

trait MemberExt {
    fn is_allow_null(&self) -> bool;
    fn validate(&self) -> Result<&dyn MemberExt, String>;
    fn format_declaration(&self) -> String;
    fn format_constructor_parameter(&self) -> String;
    fn format_constructor_implementation(&self) -> String;
    fn format_constructor_call(&self) -> String;
    fn format_getter_declaration(&self) -> String;
    fn format_setter_declaration(&self) -> String;
    fn format_database_schema(&self) -> String;
    fn format_binder_implementation(&self) -> String;
}
impl MemberExt for Member {
    fn is_allow_null(&self) -> bool { self.allow_null.unwrap_or(false) }

    fn validate(&self) -> Result<&dyn MemberExt, String> {
        if self.name.is_empty() {
            return Err("Member name is empty".to_string())
        }
        match self.type_.as_str() {
            "INT32" => { Ok(self) },
            "INT64" => { Ok(self) },
            "BYTEARRAY" => {
                if self.is_allow_null() {
                    return Err("Bytearray cannot be null".to_string())
                }
                Ok(self)
            },
            _ => Err(format!("Unexpected type: {}", self.type_))
        }
    }

    fn format_declaration(&self) -> String {
        let name = &self.name;
        let description = self.description.as_ref()
            .map(|s| String::from("        /// ") + &s + "\n")
            .unwrap_or(String::new());
        match self.type_.as_str() {
            "INT32" => {
                let type_ = if self.is_allow_null() { "std::optional<int32_t>" } else { "int32_t" };
                let declaration = format!("        {type_} _{name};");
                format!("{description}{declaration}\n")
            },
            "INT64" => {
                let type_ = if self.is_allow_null() { "std::optional<int64_t>" } else { "int64_t" };
                let declaration = format!("        {type_} _{name};");
                format!("{description}{declaration}\n")
            },
            "BYTEARRAY" => {
                let declaration = format!("        std::vector<uint8_t> _{name};");
                format!("{description}{declaration}\n")
            },
            _ => String::new()
        }
    }

    fn format_constructor_parameter(&self) -> String {
        let name = &self.name;
        match self.type_.as_str() {
            "INT32" => {
                let type_ = if self.is_allow_null() { "std::optional<int32_t>" } else { "int32_t" };
                format!("{type_} {name}")
            },
            "INT64" => {
                let type_ = if self.is_allow_null() { "std::optional<int64_t>" } else { "int64_t" };
                format!("{type_} {name}")
            },
            "BYTEARRAY" => {
                format!("std::vector<uint8_t> {name}")
            },
            _ => String::new()
        }
    }

    fn format_constructor_implementation(&self) -> String {
        let name = &self.name;
        match self.type_.as_str() {
            "BYTEARRAY" => {
                format!("_{name}(std::move({name}))")
            },
            _ => format!("_{name}({name})")
        }
    }

    fn format_constructor_call(&self) -> String {
        let name = &self.name;
        match self.type_.as_str() {
            "BYTEARRAY" => {
                format!("std::move({name})")
            },
            _ => format!("{name}")
        }
    }

    fn format_getter_declaration(&self) -> String {
        let name = &self.name;
        match self.type_.as_str() {
            "INT32" => {
                let type_ = if self.is_allow_null() { "std::optional<int32_t>" } else { "int32_t" };
                format!("        [[nodiscard]] {type_} get_{name}() const {{ return _{name}; }}\n")
            },
            "INT64" => {
                let type_ = if self.is_allow_null() { "std::optional<int64_t>" } else { "int64_t" };
                format!("        [[nodiscard]] {type_} get_{name}() const {{ return _{name}; }}\n")
            },
            "BYTEARRAY" => {
                format!("        [[nodiscard]] const std::vector<uint8_t>& get_{name}() const {{ return _{name}; }}\n")
            },
            _ => String::new()
        }
    }

    fn format_setter_declaration(&self) -> String {
        let name = &self.name;
        match self.type_.as_str() {
            "INT32" => {
                let type_ = if self.is_allow_null() { "std::optional<int32_t>" } else { "int32_t" };
                format!("        void set_{name}({type_});\n")
            },
            "INT64" => {
                let type_ = if self.is_allow_null() { "std::optional<int64_t>" } else { "int64_t" };
                format!("        void set_{name}({type_});\n")
            },
            "BYTEARRAY" => {
                format!("        void set_{name}(std::vector<uint8_t>);\n")
            },
            _ => String::new()
        }
    }

    fn format_database_schema(&self) -> String {
        let name = &self.name;
        match self.type_.as_str() {
            "INT32" | "INT64" => {
                let nullability = if self.is_allow_null() { "" } else { "NOT NULL" };
                let default_value = if self.is_allow_null() { "NULL" } else { "0" };
                format!("{name} INTEGER {nullability} DEFAULT {default_value}")
            },
            "BYTEARRAY" => {
                format!("{name} BLOC")
            },
            _ => String::new()
        }
    }

    fn format_binder_implementation(&self) -> String {
        let name = &self.name;
        if self.is_allow_null() {
            format!("{name} ? value_variant{{*{name}}} : std::monostate{{}}")
        } else {
            format!("{name}")
        }
    }
}
