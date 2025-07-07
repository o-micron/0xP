#![cfg(feature = "glsl-in")]
#![cfg(feature = "msl-out")]

use naga::*;
use std::{fs, path::PathBuf, println, slice, string::String};

pub fn parse_glsl(stage: naga::ShaderStage, inputs: &[Box<[u8]>]) {
    let mut parser = naga::front::glsl::Frontend::default();
    let options = naga::front::glsl::Options {
        stage,
        defines: Default::default(),
    };
    for input in inputs.iter() {
        let string = std::str::from_utf8(input).unwrap();
        parser.parse(&options, string).unwrap();
    }

    let mut string = String::new();
    let options = naga::back::msl::Options::default();
    for &(ref module, ref info) in inputs.iter() {
        let pipeline_options = naga::back::msl::PipelineOptions::default();
        let mut writer = naga::back::msl::Writer::new(&mut string);
        writer
            .write(module, info, &options, &pipeline_options)
            .unwrap();
        println!("{}", string.str());
        string.clear();
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {
        let mut verts = Vec::new();
        let input =
            fs::read(PathBuf::from("../prototypes/shaders/deferred/vert.glsl")).unwrap_or_default();
        verts.push(input.into_boxed_slice());
        parse_glsl(naga::ShaderStage::Vertex, &verts);
        let result = add(2, 2);
        assert_eq!(result, 4);
    }
}
