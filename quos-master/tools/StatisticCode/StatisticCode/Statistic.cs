using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace StatisticCode
{
    class Statistic
    {
        /// <summary>
        /// 总行数
        /// </summary>
        int m_iTotalCount;
        /// <summary>
        /// 统计信息
        /// </summary>
        string m_strCount;
        /// <summary>
        /// 缩进量
        /// </summary>
        int m_iIndentCount;
        /// <summary>
        /// 要处理文件的扩展名
        /// </summary>
        string[] m_strExtName;
        /// <summary>
        /// 统计所得的总行数
        /// </summary>
        public int totalCount
        {
            get
            {
                return m_iTotalCount;
            }
        }
        /// <summary>
        /// 详细信息
        /// </summary>
        public string countLog
        {
            get
            {
                return m_strCount;
            }
        }
        /// <summary>
        /// 要处理文件的扩展名数组
        /// </summary>
        public string[] ExtName
        {
            get
            {
                return m_strExtName;
            }
            set
            {
                m_strExtName = value;
            }
        }

        public Statistic()
        {
            init();
        }
        public Statistic(string[] extName)
        {
            init(extName);
        }
        /// <summary>
        /// 初始化所有成员变量
        /// </summary>
        /// <param name="extName">扩展名数组</param>
        private void init(string[] extName)
        {
            m_strExtName = extName;
            m_iTotalCount = 0;
            m_strCount = "";
            m_iIndentCount = 0;
        }
        /// <summary>
        /// 初始化除了m_strExtName以外的所有成员变量
        /// </summary>
        private void init()
        {
            m_iTotalCount = 0;
            m_strCount = "";
            m_iIndentCount = 0;
        }
        /// <summary>
        /// 统计某个目录
        /// </summary>
        /// <param name="strPath">路径名</param>
        /// <returns>true：成功；false：失败</returns>
        public bool doStatisticCode(string strPath, string[] extName)
        {
            init(extName);
            if (Directory.Exists(strPath))
            {
                dir(strPath);
                m_strCount += "总共: " + m_iTotalCount.ToString() + "行\r\n";
                return true;
            }
            else
            {
                Console.WriteLine("\t错误：目录" + strPath + "不存在！");
                return false;
            }
        }
        /// <summary>
        /// 统计某个目录
        /// </summary>
        /// <param name="strPath">路径名</param>
        /// <returns>true：成功；false：失败</returns>
        public bool doStatisticCode(string strPath)
        {
            init();
            if (Directory.Exists(strPath))
            {
                dir(strPath);
                //m_strCount += "总共: " + m_iTotalCount.ToString() + "行\r\n";
                m_strCount += "-----------------------------------------------------------------------------------------------\r\n";
                m_strCount += "                                该文件由代码统计工具自动生成\r\n";
                m_strCount += "                  如斯制作 青岛科技大学 计算机科学与技术专业 专科041班 刘胜坤";
                return true;
            }
            else
            {
                return false;
            }
        }
        /// <summary>
        /// 递归处理每个目录
        /// </summary>
        /// <param name="strPath">目录名</param>
        private void dir(string strPath)
        {
            int old = m_iTotalCount;
            string strDriMessage = strHead() + strPath + ": ";
            m_strCount += strDriMessage;
            m_iIndentCount++;
            //ConsoleColor oldColor = Console.ForegroundColor;
            //Console.ForegroundColor = ConsoleColor.Green;
            //Console.Write(strPath + "\r");
            //Console.ForegroundColor = oldColor;
            // 处理目录
            foreach (string strPathName in Directory.GetDirectories(strPath))
            {
                DirectoryInfo CurrPath = new DirectoryInfo(strPathName);
                dir(strPathName);
            }
            // 处理该目录下的文件
            file(strPath);
            m_iIndentCount--;
            old = m_iTotalCount - old;
            if (old > 0)
            {
                int i = m_strCount.IndexOf(strDriMessage);
                m_strCount = m_strCount.Insert(i + strDriMessage.Length, old.ToString() + "行\r\n");
            }
            else
            {
                int i = m_strCount.IndexOf(strDriMessage);
                m_strCount = m_strCount.Remove(i, strDriMessage.Length);
            }
        }
        /// <summary>
        /// 处理缩进
        /// </summary>
        /// <returns>缩进量</returns>
        private string strHead()
        {
            int i = m_iIndentCount;
            string strRet = "";
            while (i > 0)
            {
                strRet += "\t";
                i--;
            }
            return strRet;
        }
        /// <summary>
        /// 处理某目录下的每个文件
        /// </summary>
        /// <param name="strPath">目录名</param>
        private void file(string strPath)
        {
            foreach (string strFileName in Directory.GetFiles(strPath))
            {
                FileInfo currFileInfo = new FileInfo(strFileName);
                string strExt = currFileInfo.Extension.ToLower();
                foreach (string var in m_strExtName)
                {
                    if (strExt == var)
                    {
                        statistic(strFileName);
                        break;
                    }
                }
            }
        }
        /// <summary>
        /// 统计某个文件的行数
        /// </summary>
        /// <param name="strFileName">要统计的文件名</param>
        private void statistic(string strFileName)
        {
            string[] strFileContent = File.ReadAllLines(strFileName, Encoding.Default);
            m_iTotalCount += strFileContent.Length;
            FileInfo fi = new FileInfo(strFileName);
            m_strCount += strHead() + fi.Name + ": " + strFileContent.Length.ToString() + "行" + "\r\n";
        }
    }
}
