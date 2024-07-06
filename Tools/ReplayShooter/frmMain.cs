//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose:
//
//******************************************************************************

using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using ReplayShooter.Properties;

namespace ReplayShooter
{
    public partial class FrmMain : Form
    {
        public FrmMain()
        {
            InitializeComponent();
            txtOutputFolder.Text = Settings.Default.OutputFolder;
            searchWindows();
        }

        // Capture thread
        Thread thread = null;
        int nFile = -1;
        bool bContinue = false;
        int nFilesMax = 0;
        IntPtr hWnd = IntPtr.Zero;
        private void capture()
        {
            bContinue = true;
            try
            {
                while (nFile < nFiles.Value && bContinue)
                {
                    string file = txtOutputFolder.Text + @"\frame" + nFile.ToString("D6") + ".bmp";
                    string fileHtml = txtOutputFolder.Text + @"\frame" + nFile.ToString("D6") + ".htm";

                    if (File.Exists(file)) File.Delete(file);
                    if (!User32.IsWindowVisible(hWnd))
                        throw new Exception("Window does not exist");
                    User32.ShowWindow(hWnd, 9);
                    //User32.SetForegroundWindow(hWnd);
                    CaptureWindow(hWnd).Save(file, ImageFormat.Bmp);
                    if (File.Exists(fileHtml)) File.Delete(fileHtml);
                    string title = User32.getWindowTitle(hWnd);
                    File.WriteAllText(fileHtml, title + "\n<html><head><title>" + title + "</title></head><body><img src='" + (new FileInfo(file)).Name + "' /></body></html>");
                    nFile++;
                    Thread.Sleep((int)nDelay.Value);
                }
            }
            finally
            {
                bContinue = false;
            }
        }

        private void activateControls(bool activate)
        {
            btnChooseOutputDir.Enabled = activate;
            btnExit.Enabled = activate;
            btnWindowsRefresh.Enabled = activate;
            txtOutputFolder.Enabled = activate;
            cmbWindows.Enabled = activate;
            nFiles.Enabled = activate;
            lblStatus.Visible = activate;
            lblStatus.Text = "Saving images";
        }

        private void btnCapture_Click(object sender, EventArgs e)
        {
            if (btnCapture.Text == "Stop")
                try
                {
                    thread.Abort();
                    thread = null;
                }
                finally
                {
                    btnCapture.Text = "Start";
                    activateControls(true);
                }
            else
            {
                if (!Directory.Exists(txtOutputFolder.Text)) Directory.CreateDirectory(txtOutputFolder.Text);
                nFile = findFirstFileToWrite(txtOutputFolder.Text);
                if (nFile == -1)
                    MessageBox.Show("No more files allowed", "ERROR", MessageBoxButtons.OK, MessageBoxIcon.Error);
                else
                    if (cmbWindows.SelectedItem != null)
                    {
                        activateControls(false);
                        nFilesMax = (int)nFiles.Value;
                        hWnd = new IntPtr(int.Parse(cmbWindows.SelectedItem.ToString().Split(' ')[0], System.Globalization.NumberStyles.HexNumber));
                        btnCapture.Text = "Stop";
                        thread = new Thread(new ThreadStart(capture));
                        thread.Start();
                    }
                    else
                        MessageBox.Show("No window selected", "ERROR", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void btnChooseOutputDir_Click(object sender, EventArgs e)
        {
            if (txtOutputFolder.Text.Length > 0)
                fbDir.SelectedPath = txtOutputFolder.Text;

            if (fbDir.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                txtOutputFolder.Text = fbDir.SelectedPath;
        }

        private void btnExit_Click(object sender, EventArgs e)
        {
            if (thread != null)
                try
                {
                    thread.Abort();
                    thread = null;
                    btnCapture.Enabled = true;
                }
                finally
                { }
            else
            {
                Properties.Settings.Default.OutputFolder = txtOutputFolder.Text;
                this.Close();
            }
        }

        private void btnWindowsRefresh_Click(object sender, EventArgs e)
        {
            searchWindows();
        }

        private void searchWindows()
        {
            Dictionary<IntPtr, string> windows = (Dictionary<IntPtr, string>)User32.GetOpenWindows();
            cmbWindows.Items.Clear();
            foreach (IntPtr hWnd in windows.Keys)
                cmbWindows.Items.Add(hWnd.ToInt64().ToString("X8") + " --> " + windows[hWnd]);
        }

        private int findFirstFileToWrite(string path)
        {
            for (int i = 0; i < nFiles.Value; i++)
                if (!File.Exists(path + @"\frame" + i.ToString("D6") + ".htm"))
                    return i;
            return -1;
        }

        private Image CaptureWindow(IntPtr handle)
        {
            // get te hDC of the target window
            IntPtr hdcSrc = User32.GetWindowDC(handle);
            // get the size
            User32.RECT windowRect = new User32.RECT();
            User32.GetWindowRect(handle, ref windowRect);
            int borderSize = Math.Abs(this.Width - this.ClientRectangle.Width) / 2;
            int titleBarSize = Math.Abs(this.Height - borderSize - this.ClientRectangle.Height);
            int width = windowRect.right - windowRect.left - 2 * borderSize;
            int height = windowRect.bottom - windowRect.top - titleBarSize - borderSize;
            // create a device context we can copy to
            IntPtr hdcDest = GDI32.CreateCompatibleDC(hdcSrc);
            // create a bitmap we can copy it to,
            // using GetDeviceCaps to get the width/height
            IntPtr hBitmap = GDI32.CreateCompatibleBitmap(hdcSrc, width, height);
            // select the bitmap object
            IntPtr hOld = GDI32.SelectObject(hdcDest, hBitmap);
            // bitblt over
            //GDI32.BitBlt(hdcDest, 0, 0, width, height, hdcSrc, 0, 0, GDI32.SRCCOPY);
            GDI32.BitBlt(hdcDest, 0, 0, width, height, hdcSrc, borderSize, titleBarSize, GDI32.SRCCOPY);
            // restore selection
            GDI32.SelectObject(hdcDest, hOld);
            // clean up
            GDI32.DeleteDC(hdcDest);
            User32.ReleaseDC(handle, hdcSrc);
            // get a .NET image object for it
            Image img = Image.FromHbitmap(hBitmap);
            // free up the Bitmap object
            GDI32.DeleteObject(hBitmap);
            return img;
        }

        /// <summary>
        /// Helper class containing User32 API functions
        /// </summary>
        private class User32
        {
            [StructLayout(LayoutKind.Sequential)]
            public struct RECT
            {
                public int left;
                public int top;
                public int right;
                public int bottom;
            }

            //[DllImport("user32.dll")]
            //public static extern IntPtr GetDesktopWindow();
            [DllImport("user32.dll")]
            public static extern IntPtr GetWindowDC(IntPtr hWnd);
            [DllImport("user32.dll")]
            public static extern IntPtr ReleaseDC(IntPtr hWnd, IntPtr hDC);
            [DllImport("user32.dll")]
            public static extern IntPtr GetWindowRect(IntPtr hWnd, ref RECT rect);
            [DllImport("USER32.DLL")]
            private static extern int GetWindowText(IntPtr hWnd, StringBuilder lpString, int nMaxCount);
            [DllImport("USER32.DLL")]
            private static extern int GetWindowTextLength(IntPtr hWnd);
            [DllImport("USER32.DLL")]
            public static extern bool IsWindowVisible(IntPtr hWnd);

            private delegate bool EnumWindowsProc(IntPtr hWnd, int lParam);
            [DllImport("USER32.DLL")]
            private static extern bool EnumWindows(EnumWindowsProc enumFunc, int lParam);
            [DllImport("USER32.DLL")]
            private static extern IntPtr GetShellWindow();
            [DllImport("user32.dll")]
            public static extern int ShowWindow(IntPtr hwnd, int nCmdShow);
            [DllImport("user32.dll", EntryPoint = "SetForegroundWindow")]
            public static extern bool SetForegroundWindow(IntPtr hWnd);

            private const int SW_RESTORE = 9;		// Restore the window (not maximized nor minimized). 

            private static IntPtr shellWindow = IntPtr.Zero;
            private static Dictionary<IntPtr, string> windows = new Dictionary<IntPtr, string>();

            private static bool _fEnumWindowsCallBack(IntPtr hWnd, int lParam)
            {
                if (hWnd == shellWindow) return true;
                if (!IsWindowVisible(hWnd)) return true;

                string title = getWindowTitle(hWnd);
                if (title.Length == 0) return true;

                windows[hWnd] = title;
                return true;
            }

            public static IDictionary<IntPtr, string> GetOpenWindows()
            {
                shellWindow = GetShellWindow();
                windows.Clear();
                EnumWindowsProc fEnumWindowsCallBack = new EnumWindowsProc(_fEnumWindowsCallBack);
                EnumWindows(fEnumWindowsCallBack, 0);
                return windows;
            }

            public static string getWindowTitle(IntPtr hWnd)
            {
                int length = GetWindowTextLength(hWnd);
                if (length == 0) return "";

                StringBuilder builder = new StringBuilder(length);
                GetWindowText(hWnd, builder, length + 1);

                return builder.ToString();
            }
        }

        /// <summary>
        /// Helper class containing Gdi32 API functions
        /// </summary>
        private class GDI32
        {

            public const int SRCCOPY = 0x00CC0020; // BitBlt dwRop parameter

            [DllImport("gdi32.dll")]
            public static extern bool BitBlt(IntPtr hObject, int nXDest, int nYDest,
                int nWidth, int nHeight, IntPtr hObjectSource,
                int nXSrc, int nYSrc, int dwRop);
            [DllImport("gdi32.dll")]
            public static extern IntPtr CreateCompatibleBitmap(IntPtr hDC, int nWidth,
                int nHeight);
            [DllImport("gdi32.dll")]
            public static extern IntPtr CreateCompatibleDC(IntPtr hDC);
            [DllImport("gdi32.dll")]
            public static extern bool DeleteDC(IntPtr hDC);
            [DllImport("gdi32.dll")]
            public static extern bool DeleteObject(IntPtr hObject);
            [DllImport("gdi32.dll")]
            public static extern IntPtr SelectObject(IntPtr hDC, IntPtr hObject);
        }
    }
}
