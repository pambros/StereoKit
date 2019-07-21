﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace StereoKit
{
    public class Shader
    {
        #region Imports
        [DllImport(Util.DllName, CharSet = CharSet.Ansi)]
        static extern IntPtr shader_create(string hlsl);
        [DllImport(Util.DllName, CharSet = CharSet.Ansi)]
        static extern IntPtr shader_create_file(string file);
        [DllImport(Util.DllName)]
        static extern void shader_destroy(IntPtr shader);
        #endregion

        internal IntPtr _shaderInst;
        private Shader()
        {
        }
        public Shader(string file)
        {
            _shaderInst = shader_create_file(file);
            if (_shaderInst == IntPtr.Zero)
                Console.WriteLine("Couldn't load shader file {0}!", file);
        }
        ~Shader()
        {
            if (_shaderInst == IntPtr.Zero)
                shader_destroy(_shaderInst);
        }

        public static Shader FromHLSL(string hlsl)
        {
            Shader result = new Shader();
            result._shaderInst = shader_create(hlsl);
            return result;
        }
    }
}