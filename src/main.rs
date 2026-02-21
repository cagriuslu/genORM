mod cxx;
mod util;

use std::env;
use std::fs::File;
use std::io::BufReader;
use serde::Deserialize;
use crate::cxx::generate;

#[derive(Deserialize, Debug)]
struct CxxOptions {
    #[serde(rename = "file-prefix")]
    file_prefix: String,

    namespace: String,

    #[serde(rename = "output-dir-root")]
    output_dir_root: Option<String>,

    #[serde(rename = "output-dir")]
    output_dir: Option<String>,
}

#[derive(Deserialize, Debug)]
struct Member {
    name: String,

    description: Option<String>,

    #[serde(rename = "type")]
    type_: String,

    #[serde(rename = "allow-null")]
    allow_null: Option<bool>,
    
    index: Option<bool>
}

#[derive(Deserialize, Debug)]
struct ObjectType {
    name: String,
    description: Option<String>,
    members: Vec<Member>,
}

#[derive(Deserialize, Debug)]
struct Config {
    #[serde(rename = "genORM-config-version")]
    config_version: u32,

    #[serde(rename = "cxx-options")]
    cxx_options: Option<CxxOptions>,

    #[serde(rename = "object-types")]
    object_types: Vec<ObjectType>,
}

fn main() {
    // Ignore first arg
    let mut args = env::args().skip(1);
    let fpath = match args.next() {
        None => {
            eprintln!("Usage: genORM CONFIG-JSON");
            std::process::exit(1);
        },
        Some(s) => { s }
    };

    let file = match File::open(&fpath) {
        Ok(f) => f,
        Err(e) => {
            eprintln!("Unable to open: {}, reason: {}", fpath, e);
            std::process::exit(1);
        }
    };
    let reader = BufReader::new(file);
    let config : Result<Config, serde_json::Error> = serde_json::from_reader(reader);
    match config {
        Ok(config) => {
            if config.config_version != 1 {
                eprintln!("[WARN] Unsupported config version")
            }
            if let Some(cxx_options) = config.cxx_options {
                let output_dir = util::calculate_output_dir(&cxx_options.output_dir_root, &cxx_options.output_dir);
                match output_dir {
                    Ok(output_dir) => {
                        if let Err(e) = generate(output_dir, &cxx_options, &config.object_types) {
                            eprintln!("Error while generating C++ outputs: {:?}", e);
                            std::process::exit(1);
                        }
                    }
                    Err(e) => {
                        eprintln!("Error while calculating output dir: {:?}", e);
                        std::process::exit(1);
                    }
                }
            }
        },
        Err(e) => {
            eprintln!("Error while loading config: {:?}", e);
            std::process::exit(1);
        }
    }
}