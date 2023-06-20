using System;
using System.IO;
using System.Collections.Generic;
using System.Text;
using System.Threading;

namespace StatisticCode
{
    class Program
    {            
        static int Main(string[] args)
        {
            string strPath = @".\";
            string iniFileName = "extName.ini";
            bool bRet = false;
            string[] strExtName;
            Statistic sta = new Statistic();
            DirectoryInfo dirInfo = new DirectoryInfo(strPath);
            string outFileName = dirInfo.Name + ".txt";
#region 参数分析
            if (args.Length == 1 || args.Length > 3)
            {
                Console.WriteLine("错误：参数过多或过少！");
                help(iniFileName, outFileName);
                return 1;
            }
            if (args.Length >= 2)
            {
                strPath = args[0];
                if (!Directory.Exists(strPath))
                {
                    Console.WriteLine("错误：路径" + strPath + "不存在！");
                    help(iniFileName, outFileName);
                    return 1;
                }
                dirInfo = new DirectoryInfo(strPath);
                outFileName = args[1];
            }

            if (args.Length == 3)
            {
                iniFileName = args[2];
            }

            // 扩展名配置文件分析
            if (File.Exists(iniFileName))
            {
                strExtName = File.ReadAllLines(iniFileName, Encoding.Default);
            }
            else
            {
                strExtName = new string[] { ".c", ".cpp", ".cc", ".h", ".java", ".cs", ".vb", ".s", ".asm", ".inc", ".js", ".vbs", ".asp", ".aspx", ".jsp", ".php", ".py", ".htm", ".html", ".css" };
                Console.Write("在" + dirInfo.Name + "没有找到配置文件" + iniFileName + "。默认统计以下扩展名类型的文件: \r\n\t");
                // 生成一个新的配置文件
                iniFileName = "extName.ini";
                StreamWriter sw = File.CreateText(iniFileName);
                foreach (string var in strExtName)
                {
                    Console.Write(var + "  ");
                    sw.Write(var + "\r\n");
                }
                sw.Close();
            }
            sta.ExtName = strExtName;
#endregion
            // 开始统计
            while (!bRet)
            {
                if (strPath == @".\")
                {
                    Console.Write("\r\n请输入目录名[默认:" + dirInfo.Name + "]：");
                    strPath = Console.ReadLine();
                    if (strPath.Length < 1) strPath = @".\";// 处理直接输出回车
                }
                if (!Directory.Exists(strPath))
                {
                    Console.WriteLine("\t错误：目录" + strPath + "不存在！");
                    bRet = false;
                }
                else
                {
                    bRet = sta.doStatisticCode(strPath);
                    if (!bRet) strPath = @".\";
                }
            }
#region 输出结果
            if (outFileName == "")//如果没有指定输出文件名就使用目录名
            {
                outFileName = dirInfo.Name + ".txt";
            }
            string str = sta.countLog;
            if (sta.totalCount <= 0)
            {
                str = "在" + dirInfo.Name + "目录中没有找到扩展名为";
                foreach (string var in strExtName)
                {
                    str += var + ";";
                }
                str += "的文件！";
            }
            // 保存到文件中
            StreamWriter swr = File.CreateText(outFileName);
            swr.Write(str);
            swr.Close();
            // 在控制台输出结果
            Console.WriteLine(str);
#endregion
            return 0;
        }

        private static void help(string iniFileName, string outFileName)
        {
            Console.WriteLine("命令行运行参数：StatisticCode 目录名 输出文件名 [配置文件名] ");
            Console.WriteLine("\t 目录名： \t要统计的目录名");
            Console.WriteLine("\t 输出文件名：\t将统计结果输出到该文件中。\r\n\t\t\t默认：" + outFileName);
            Console.WriteLine("\t 配置文件：\t每行一个扩展名，可以有多个行。\r\n\t\t\t默认：" + iniFileName);
        }
 
    }
}
