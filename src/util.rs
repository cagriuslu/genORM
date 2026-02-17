
pub fn calculate_output_dir(root: &Option<String>, dir: &Option<String>) -> Result<String, &'static str> {
    let mut root_dir = String::new();
    if let Some(root) = root {
        match root.as_str() {
            "GIT_ROOT" => {
                // TODO
                root_dir = String::from("./");
            },
            _ => return Err("Unknown root")
        }
    }

    if let Some(dir) = dir {
        return Ok(root_dir + &dir);
    }
    return Ok(root_dir);
}