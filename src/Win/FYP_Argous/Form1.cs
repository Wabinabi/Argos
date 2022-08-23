using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;


namespace FYP_Argous
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private Process pDocked;
        private IntPtr hWndOriginalParent;
        private IntPtr hWndDocked;
        private IntPtr hWndParent;


        [System.Runtime.InteropServices.DllImport("user32.dll")]
        static extern IntPtr SetParent(IntPtr hWndChild, IntPtr hWndNewParent);

        [System.Runtime.InteropServices.DllImport("user32.dll", SetLastError = true)]
        public static extern bool MoveWindow(IntPtr hWnd, int X, int Y, int nWidth, int nHeight, bool bRepaint);

        private void button1_Click(object sender, EventArgs e)
        {
            if (hWndDocked != IntPtr.Zero) //don't do anything if there's already a window docked.
                return;

            hWndParent = IntPtr.Zero;




            OpenFileDialog od = new OpenFileDialog();
            if (od.ShowDialog() == DialogResult.OK)
            {
                pDocked = Process.Start(od.FileName);

            }

            while (hWndDocked == IntPtr.Zero)
            {
                pDocked.WaitForInputIdle(10); //wait for the window to be ready for input;
                pDocked.Refresh();              //update process info
                if (pDocked.HasExited)
                {
                    return; //abort if the process finished before we got a handle.
                }
                hWndDocked = pDocked.MainWindowHandle;  //cache the window handle
            }
            

            hWndOriginalParent = SetParent(hWndDocked, panel1.Handle);


            //Windows API call to change the parent of the target window.
            //It returns the hWnd of the window's parent prior to this call.



            //OpenFileDialog od = new OpenFileDialog();
            //if (od.ShowDialog() == DialogResult.OK)
            //{
            //    Process proc = Process.Start(od.FileName);
            //    proc.WaitForInputIdle();

            //    //while (proc.MainWindowHandle == IntPtr.Zero)
            //    //{
            //    //    Thread.Sleep(1000);
            //    //    proc.Refresh();

            //    //}

            //    SetParent(proc.MainWindowHandle, this.panel1.Handle);

            //}

        }

        private void openFileDialog1_FileOk(object sender, CancelEventArgs e)
        {

        }

        private void button2_Click(object sender, EventArgs e)
        {
            System.IO.Stream st;
            OpenFileDialog d1 = new OpenFileDialog();

            if (d1.ShowDialog() == DialogResult.OK)
            {
                if ((st = d1.OpenFile()) != null)
                {
                    string file = d1.FileName;
                    string str = File.ReadAllText(file);
                    textBox1.Text = str;
                }
            }

        }
    }
}
