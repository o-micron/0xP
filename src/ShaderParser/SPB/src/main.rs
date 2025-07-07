use naga::{back::wgsl::WriterFlags, valid::Capabilities, valid::ModuleInfo, Module};
use std::{fs, path::PathBuf, println, slice, string::String};

fn type_to_str(t: &naga::TypeInner) -> String {
    match t {
        naga::TypeInner::Scalar { kind, width: _ } => match kind {
            naga::ScalarKind::Sint => return String::from("int"),
            naga::ScalarKind::Uint => return String::from("uint"),
            naga::ScalarKind::Float => return String::from("float"),
            naga::ScalarKind::Bool => return String::from("bool"),
        },
        naga::TypeInner::Vector {
            size,
            kind,
            width: _,
        } => {
            let k = match kind {
                naga::ScalarKind::Sint => "int",
                naga::ScalarKind::Uint => "uint",
                naga::ScalarKind::Float => "float",
                naga::ScalarKind::Bool => "bool",
            };
            let s = match size {
                naga::VectorSize::Bi => "2",
                naga::VectorSize::Tri => "3",
                naga::VectorSize::Quad => "4",
            };
            return format!("{}{}", k, s);
        }
        naga::TypeInner::Sampler { comparison: _ } => return String::from("texture2d<float>"),
        naga::TypeInner::Matrix {
            columns,
            rows,
            width: _,
        } => {
            let c = match columns {
                naga::VectorSize::Bi => "2",
                naga::VectorSize::Tri => "3",
                naga::VectorSize::Quad => "4",
            };
            let r = match rows {
                naga::VectorSize::Bi => "2",
                naga::VectorSize::Tri => "3",
                naga::VectorSize::Quad => "4",
            };
            return format!("matrix_float{}x{}", c, r);
        }
        _ => return String::from(""),
    };
}

pub fn parse_glsl(stage: naga::ShaderStage, inputs: &[Box<[u8]>]) {
    for input in inputs.iter() {
        let input_source = std::str::from_utf8(input).unwrap();
        let mut parser = naga::front::glsl::Parser::default();
        let glsl_options = naga::front::glsl::Options::from(stage);
        let module = parser.parse(&glsl_options, input_source).unwrap();
        let mut capabilities = Capabilities::empty();
        capabilities |= Capabilities::PUSH_CONSTANT
            | Capabilities::FLOAT64
            | Capabilities::PRIMITIVE_INDEX
            | Capabilities::SAMPLED_TEXTURE_AND_STORAGE_BUFFER_ARRAY_NON_UNIFORM_INDEXING
            | Capabilities::UNIFORM_BUFFER_AND_STORAGE_TEXTURE_ARRAY_NON_UNIFORM_INDEXING
            | Capabilities::SAMPLER_NON_UNIFORM_INDEXING
            | Capabilities::CLIP_DISTANCE
            | Capabilities::CULL_DISTANCE
            | Capabilities::STORAGE_TEXTURE_16BIT_NORM_FORMATS;

        let module_info =
            naga::valid::Validator::new(naga::valid::ValidationFlags::default(), capabilities)
                .validate(&module)
                .unwrap();
        let msl_options = naga::back::msl::Options::default();
        let mut output = String::new();
        let pipeline_options = naga::back::msl::PipelineOptions::default();
        let mut writer = naga::back::msl::Writer::new(&mut output);
        writer
            .write(&module, &module_info, &msl_options, &pipeline_options)
            .unwrap();
        println!(
            "entry_points: {}\nglobal_variables: {}\nfunctions: {}",
            module
                .entry_points
                .iter()
                .map(|entry_point| { String::from(entry_point.name.as_str()) })
                .collect::<Vec<String>>()
                .join(","),
            module
                .global_variables
                .iter()
                .map(|global_variable| {
                    String::from(
                        global_variable
                            .clone()
                            .1
                            .clone()
                            .name
                            .clone()
                            .unwrap_or_default(),
                    )
                })
                .collect::<Vec<String>>()
                .join(","),
            module
                .functions
                .iter()
                .map(|func| {
                    let f = func.clone().1.clone();
                    let fname = String::from(f.name.clone().unwrap_or_default());
                    return format!(
                        "{}({})",
                        fname,
                        f.arguments
                            .iter()
                            .map(|farg| {
                                let t = &module.types[farg.ty];
                                let type_name = type_to_str(&t.inner);
                                format!(
                                    "{}: {:?}",
                                    &farg.name.clone().unwrap_or_default(),
                                    type_name
                                )
                            })
                            .collect::<Vec<String>>()
                            .join(",")
                    );
                })
                .collect::<Vec<String>>()
                .join(",")
        );
    }
}

pub fn start() {
    for p in [
        "/Users/omar/code/XP/src/prototypes/shaders/functions/functions.glsl",
        //        "/Users/omar/code/XP/src/prototypes/shaders/functions/EdgeFilter.glsl",
        // "/Users/omar/code/XP/src/prototypes/shaders/functions/HappyJumping.glsl",
    ] {
        let mut verts = Vec::new();
        let input = fs::read(PathBuf::from(p)).unwrap_or_default();
        verts.push(input.into_boxed_slice());
        parse_glsl(naga::ShaderStage::Fragment, &verts);
    }
}

fn main() {
    start();
}
