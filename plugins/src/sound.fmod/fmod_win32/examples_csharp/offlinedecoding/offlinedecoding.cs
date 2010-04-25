/*===============================================================================================
 Offline Decoding Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2008.

 This example shows how decode a file to PCM, without playing it.
 It writes out the data as a raw data file.
 The System::createSound function uses FMOD_OPENONLY so that FMOD does not read any data 
 itself.
 If this is uses then it is up to the user to use Sound::readData to get the data out of 
 the file and into the destination buffer.
===============================================================================================*/

using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.Runtime.InteropServices;
using System.IO;

namespace offlinedecoding
{
	public class Form1 : System.Windows.Forms.Form
	{
        private const int CHUNKSIZE = 4096;

        private FMOD.System  system  = null;
        private FMOD.Sound   sound   = null;

        private System.Windows.Forms.StatusBar statusBar;
        private System.Windows.Forms.Button exit_button;
        private System.Windows.Forms.Button playButton;
        private System.Windows.Forms.Label label;

		private System.ComponentModel.Container components = null;

		public Form1()
		{
			InitializeComponent();
		}

		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
            this.statusBar = new System.Windows.Forms.StatusBar();
            this.exit_button = new System.Windows.Forms.Button();
            this.playButton = new System.Windows.Forms.Button();
            this.label = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // statusBar
            // 
            this.statusBar.Location = new System.Drawing.Point(0, 99);
            this.statusBar.Name = "statusBar";
            this.statusBar.Size = new System.Drawing.Size(264, 24);
            this.statusBar.TabIndex = 18;
            // 
            // exit_button
            // 
            this.exit_button.Location = new System.Drawing.Point(96, 72);
            this.exit_button.Name = "exit_button";
            this.exit_button.Size = new System.Drawing.Size(72, 24);
            this.exit_button.TabIndex = 19;
            this.exit_button.Text = "Exit";
            this.exit_button.Click += new System.EventHandler(this.exit_button_Click);
            // 
            // playButton
            // 
            this.playButton.Location = new System.Drawing.Point(64, 32);
            this.playButton.Name = "playButton";
            this.playButton.Size = new System.Drawing.Size(136, 32);
            this.playButton.TabIndex = 20;
            this.playButton.Text = "Start";
            this.playButton.Click += new System.EventHandler(this.playButton_Click);
            // 
            // label
            // 
            this.label.Location = new System.Drawing.Point(0, 0);
            this.label.Name = "label";
            this.label.Size = new System.Drawing.Size(264, 32);
            this.label.TabIndex = 21;
            this.label.Text = "Copyright (c) Firelight Technologies 2004 - 2008";
            this.label.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Form1
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(264, 123);
            this.Controls.Add(this.label);
            this.Controls.Add(this.playButton);
            this.Controls.Add(this.exit_button);
            this.Controls.Add(this.statusBar);
            this.Name = "Form1";
            this.Text = "Offline Decoding Example";
            this.ResumeLayout(false);

        }
		#endregion

		[STAThread]
		static void Main() 
		{
			Application.Run(new Form1());
		}

        private void playButton_Click(object sender, System.EventArgs e)
        {
            uint            version = 0;
            FMOD.RESULT     result;

            /*
                Global Settings
            */
            result = FMOD.Factory.System_Create(ref system);
            ERRCHECK(result);

            result = system.getVersion(ref version);
            ERRCHECK(result);
            if (version < FMOD.VERSION.number)
            {
                MessageBox.Show("Error!  You are using an old version of FMOD " + version.ToString("X") + ".  This program requires " + FMOD.VERSION.number.ToString("X") + ".");
                Application.Exit();
            }

            result = system.init(1, FMOD.INITFLAG.NORMAL, (IntPtr)null);
            ERRCHECK(result);       

            result = system.createSound("../../../../examples/media/wave.mp3", FMOD.MODE.OPENONLY | FMOD.MODE.ACCURATETIME, ref sound);
            ERRCHECK(result);
       
            /*
                Decode the sound and write it to a .raw file.
            */
            {
                IntPtr  data = Marshal.AllocHGlobal(CHUNKSIZE);
                byte[]  buffer = new byte[CHUNKSIZE];
                uint    length = 0, read = 0;
                uint    bytesread = 0;
                
                FileStream fs = new FileStream("output.raw", FileMode.Create, FileAccess.Write);

                result = sound.getLength(ref length, FMOD.TIMEUNIT.PCMBYTES);
                ERRCHECK(result);

                bytesread = 0;
                do
                {
                    result = sound.readData(data, CHUNKSIZE, ref read);

                    Marshal.Copy(data, buffer, 0, CHUNKSIZE);

                    fs.Write(buffer, 0, (int)read);

                    bytesread += read;

                    statusBar.Text = "writing " + bytesread + " bytes of " + length + " to output.raw";
                }
                while(result == FMOD.RESULT.OK && read == CHUNKSIZE);

                statusBar.Text = "done";

                /*
                    Loop terminates when either 
                    1. the read function returns an error.  (ie FMOD_ERR_FILE_EOF etc).
                    2. the amount requested was different to the amount returned. (somehow got an EOF without the file error, maybe a non stream file format like mod/s3m/xm/it/midi).

                    If 'bytesread' is bigger than 'length' then it just means that FMOD miscalculated the size, 
                    but this will not usually happen if FMOD_ACCURATETIME is used.  (this will give the correct length for VBR formats)
                */

                fs.Close();

            
                /*
                    Shut down
                */
                if (sound != null)
                {
                    result = sound.release();
                    ERRCHECK(result);
                }
                if (system != null)
                {
                    result = system.close();
                    ERRCHECK(result);
                    result = system.release();
                    ERRCHECK(result);
                }
            }
        }

        private void exit_button_Click(object sender, System.EventArgs e)
        {
            Application.Exit();
        }

        private void ERRCHECK(FMOD.RESULT result)
        {
            if (result != FMOD.RESULT.OK)
            {
                MessageBox.Show("FMOD error! " + result + " - " + FMOD.Error.String(result));
                Environment.Exit(-1);
            }
        }
	}
}
