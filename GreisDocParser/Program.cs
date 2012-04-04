﻿using System;
using System.Collections;
using System.Collections.Generic;
using System.Configuration;
using System.IO;
using System.Linq;
using System.Windows.Forms;

namespace GreisDocParser
{
    static class Program
    {
        public static string MetaInfoFile { get; set; }
        public static string OutputDir { get; set; }

        public const string BaselineTemplateFileKey = "baselineTemplateFile";
        public const string PlainTextReferenceFileKey = "PlainTextReferenceFile";
        public const string MetaInfoFileKey = "MetaInfoFile";
        public const string OutputDirKey = "OutputDir";
        public const string CppEnvTemplatesDirKey = "CppEnvTemplatesDir";

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            {
                //List<string> codePairs = new List<string>();
                /*var codePairs = new Dictionary<string,int>();
                string lastCode = null;
                var file = File.OpenText(
                    @"D:\Documents\svn_ifz_ipg\Projects\trunk\src\JpsParser\DataSample\ipg_2011_03_28_00.00.00.jps");
                while (!file.EndOfStream)
                {
                    var line = file.ReadLine();
                    if (line.Length > 5)
                    {
                        var thisCode = line.Substring(0, 2);
                        if (lastCode != null)
                        {
                            var key = lastCode + thisCode;
                            int val;
                            if (codePairs.TryGetValue(key, out val))
                            {
                                codePairs[key] = val + 1;
                            } else
                            {
                                codePairs[key] = 1;
                            }
                        }
                        lastCode = thisCode;
                    }
                }
                GC.Collect();

                var cc = codePairs.Where(p => p.Key.All(c => c > 48 && c < 126)).OrderByDescending(p => p.Value).Select(p => p.Key + "=>" + p.Value).ToList();
                //var sample = File.ReadAllText(@"D:\Documents\svn_ifz_ipg\Projects\trunk\src\JpsParser\DataSample\ipg_2011_03_28_00.00.00.jps");
                //var lines = sample.Split(new[] {"\r\n"}, StringSplitOptions.RemoveEmptyEntries);
                return;*/
            }

            MetaInfoFile = GetDefaultPathForKey(MetaInfoFileKey);
            OutputDir = GetDefaultPathForKey(OutputDirKey);

            try
            {
                var args = Environment.GetCommandLineArgs();
                if (args.Length > 1)
                {
                    // Command line mode
                    var command = args[1];
                    switch (command)
                    {
                    case "generate":
                        if (args.Length != 3)
                        {
                            Console.WriteLine("Invalid subcommand, type 'app.exe help' for detailed help.");
                            return;
                        }
                        var subCommand = args[2];
                        switch (subCommand)
                        {
                        case "cppEnv":
                            Console.WriteLine("C++ environment generating...");
                            GenerateCppEnv();
                            Console.WriteLine("Done!");
                            break;
                        case "sqlBaseline":
                            Console.WriteLine("SQL baseline generating...");
                            GenerateBaseline();
                            Console.WriteLine("Done!");
                            break;
                        default:
                            Console.WriteLine("Invalid subcommand, type 'app.exe help' for detailed help.");
                            return;
                        }
                        return;
                    default:
                        Console.WriteLine("Usage: 'app.exe <command> <subCommand>',\r\n" +
                                          "\r\n" +
                                          "Where:\r\n" +
                                          "    <command> - one of the following:\r\n" +
                                          "        generate - generate action;\r\n" +
                                          "        help - this help.\r\n" +
                                          "    <subcommand> - depends on command.\r\n" +
                                          "        for generate:\r\n" +
                                          "            cppEnv - generate cpp environment;\r\n" +
                                          "            sqlBaseline - generate MySql baseline.\r\n" +
                                          "\r\n");
                        return;
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Unexpected exception: {0}", ex);
                return;
            }

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new MainForm());
        }

        public static void GenerateCppEnv()
        {
            // take meta-info
            var metaInfo = MetaInfo.FromXmlFile(MetaInfoFile);

            var cppEnvTemplatesDir = Program.GetDefaultPathForKey(CppEnvTemplatesDirKey);
            var outDir = OutputDir + @"\Greis";

            var cppGen = new CppEnvironmentGenerator(metaInfo, cppEnvTemplatesDir);
            cppGen.Generate(outDir);
        }

        public static string GetDefaultPathForKey(string key)
        {
            var path = ConfigurationManager.AppSettings[key];
            if (String.IsNullOrEmpty(path))
            {
                return "";
            }
            if (path.StartsWith("\\"))
            {
                path = path.Substring(1);
            }
            if (!Path.IsPathRooted(path))
            {
                path = Path.Combine(Application.StartupPath, path);
            }
            path = Path.GetFullPath(path);
            return path;
        }

        public static void GenerateBaseline()
        {
            // take meta-info
            var metaInfo = MetaInfo.FromXmlFile(MetaInfoFile);

            // take template file path
            var baselineTemplateFile = GetDefaultPathForKey(BaselineTemplateFileKey);

            // generate script
            var baselineGenerator = new MysqlBaselineGenerator(metaInfo, baselineTemplateFile, "world");
            baselineGenerator.GenerateMysqlBaseline(Path.Combine(OutputDir, "baseline.sql"));
        }
    }
}
