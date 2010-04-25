/*===============================================================================================
 User Created Sound Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2008.

 This example shows how create a sound with data filled by the user.
 It shows a user created static sample, followed by a user created stream.
 The former allocates all memory needed for the sound and is played back as a static sample, 
 while the latter streams the data in chunks as it plays, using far less memory.
===============================================================================================*/

using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.Runtime.InteropServices;

namespace usercreatedsound
{
    public class UserCreatedSound : System.Windows.Forms.Form
	{
        private FMOD.System    system   = null;
        private FMOD.Sound     sound    = null;
        private FMOD.Channel   channel  = null;
        private FMOD.CREATESOUNDEXINFO createsoundexinfo = new FMOD.CREATESOUNDEXINFO();
        private bool           soundcreated = false;
        FMOD.MODE              mode = (FMOD.MODE._2D | FMOD.MODE.DEFAULT | FMOD.MODE.OPENUSER | FMOD.MODE.LOOP_NORMAL | FMOD.MODE.HARDWARE);

        private System.Windows.Forms.Label label;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Button exit_button;
        private System.Windows.Forms.Button pause;
        private System.Windows.Forms.Timer timer;
        private System.Windows.Forms.StatusBar statusBar;
        private System.ComponentModel.IContainer components;

		public UserCreatedSound()
		{
			InitializeComponent();
		}

		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
                FMOD.RESULT     result;
            
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
            this.components = new System.ComponentModel.Container();
            this.label = new System.Windows.Forms.Label();
            this.button1 = new System.Windows.Forms.Button();
            this.button2 = new System.Windows.Forms.Button();
            this.exit_button = new System.Windows.Forms.Button();
            this.pause = new System.Windows.Forms.Button();
            this.timer = new System.Windows.Forms.Timer(this.components);
            this.statusBar = new System.Windows.Forms.StatusBar();
            this.SuspendLayout();
            // 
            // label
            // 
            this.label.Location = new System.Drawing.Point(8, 8);
            this.label.Name = "label";
            this.label.Size = new System.Drawing.Size(264, 32);
            this.label.TabIndex = 6;
            this.label.Text = "Copyright (c) Firelight Technologies 2004-2008";
            this.label.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(8, 48);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(264, 40);
            this.button1.TabIndex = 7;
            this.button1.Text = "Play as a runtime decoded stream";
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(8, 96);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(264, 40);
            this.button2.TabIndex = 8;
            this.button2.Text = "Play as a static in memory sample";
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // exit_button
            // 
            this.exit_button.Location = new System.Drawing.Point(104, 192);
            this.exit_button.Name = "exit_button";
            this.exit_button.Size = new System.Drawing.Size(72, 24);
            this.exit_button.TabIndex = 13;
            this.exit_button.Text = "Exit";
            this.exit_button.Click += new System.EventHandler(this.exit_button_Click);
            // 
            // pause
            // 
            this.pause.Location = new System.Drawing.Point(8, 144);
            this.pause.Name = "pause";
            this.pause.Size = new System.Drawing.Size(264, 40);
            this.pause.TabIndex = 14;
            this.pause.Text = "Pause / Resume";
            this.pause.Click += new System.EventHandler(this.pause_Click);
            // 
            // timer
            // 
            this.timer.Enabled = true;
            this.timer.Interval = 10;
            this.timer.Tick += new System.EventHandler(this.timer_Tick);
            // 
            // statusBar
            // 
            this.statusBar.Location = new System.Drawing.Point(0, 219);
            this.statusBar.Name = "statusBar";
            this.statusBar.Size = new System.Drawing.Size(280, 24);
            this.statusBar.TabIndex = 15;
            // 
            // UserCreatedSound
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(280, 243);
            this.Controls.Add(this.statusBar);
            this.Controls.Add(this.pause);
            this.Controls.Add(this.exit_button);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.label);
            this.Name = "UserCreatedSound";
            this.Text = "User Created Sound Example";
            this.Load += new System.EventHandler(this.UserCreatedSound_Load);
            this.ResumeLayout(false);

        }
		#endregion

		[STAThread]
		static void Main() 
		{
			Application.Run(new UserCreatedSound());
		}
        
        private FMOD.SOUND_PCMREADCALLBACK   pcmreadcallback =   new FMOD.SOUND_PCMREADCALLBACK(PCMREADCALLBACK);
        private FMOD.SOUND_PCMSETPOSCALLBACK pcmsetposcallback = new FMOD.SOUND_PCMSETPOSCALLBACK(PCMSETPOSCALLBACK);

        private void UserCreatedSound_Load(object sender, System.EventArgs e)
        {
            uint        version = 0;
            FMOD.RESULT result;
            uint        channels = 2, frequency = 44100;

            /*
                Create a System object and initialize.
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
            result = system.init(32, FMOD.INITFLAG.NORMAL, (IntPtr)null);
            ERRCHECK(result);

            createsoundexinfo.cbsize            = Marshal.SizeOf(createsoundexinfo);
            createsoundexinfo.fileoffset        = 0;
            createsoundexinfo.length            = frequency * channels * 2 * 2;
            createsoundexinfo.numchannels       = (int)channels;
            createsoundexinfo.defaultfrequency  = (int)frequency;
            createsoundexinfo.format            = FMOD.SOUND_FORMAT.PCM16;
            createsoundexinfo.pcmreadcallback   = pcmreadcallback;
            createsoundexinfo.pcmsetposcallback = pcmsetposcallback;
            createsoundexinfo.dlsname           = null;
        }

        private void button1_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;

            if(!soundcreated)
            {
                result = system.createSound(
                    (string)null, 
                    (mode | FMOD.MODE.CREATESTREAM), 
                    ref createsoundexinfo,
                    ref sound);
                ERRCHECK(result);

                soundcreated = true;
            }
            system.playSound(FMOD.CHANNELINDEX.FREE, sound, false, ref channel);
        }

        private void button2_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;

            if(!soundcreated)
            {
                result = system.createSound(
                    (string)null, 
                    mode,
                    ref createsoundexinfo,
                    ref sound);
                ERRCHECK(result);

                soundcreated = true;
            }
            system.playSound(FMOD.CHANNELINDEX.FREE, sound, false, ref channel);            
        }

        private void pause_Click(object sender, System.EventArgs e)
        {
            bool paused = false;

            if (channel != null)
            {
                channel.getPaused(ref paused);
                channel.setPaused(!paused);
            }
        }

        private void exit_button_Click(object sender, System.EventArgs e)
        {
            Application.Exit();
        }

        private void timer_Tick(object sender, System.EventArgs e)
        {
            if (channel != null)
            {
                FMOD.RESULT result;
                uint        ms      = 0;
                uint        lenms   = 0;
                bool        playing = false;
                bool        paused  = false;

                result = channel.isPlaying(ref playing);
                if ((result != FMOD.RESULT.OK) && (result != FMOD.RESULT.ERR_INVALID_HANDLE) && (result != FMOD.RESULT.ERR_CHANNEL_STOLEN))
                {
                    ERRCHECK(result);
                }

                result = channel.getPaused(ref paused);
                if ((result != FMOD.RESULT.OK) && (result != FMOD.RESULT.ERR_INVALID_HANDLE) && (result != FMOD.RESULT.ERR_CHANNEL_STOLEN))
                {
                    ERRCHECK(result);
                }

                result = channel.getPosition(ref ms, FMOD.TIMEUNIT.MS);
                if ((result != FMOD.RESULT.OK) && (result != FMOD.RESULT.ERR_INVALID_HANDLE) && (result != FMOD.RESULT.ERR_CHANNEL_STOLEN))
                {
                    ERRCHECK(result);
                }

                result = sound.getLength( ref lenms, FMOD.TIMEUNIT.MS);
                if ((result != FMOD.RESULT.OK) && (result != FMOD.RESULT.ERR_INVALID_HANDLE) && (result != FMOD.RESULT.ERR_CHANNEL_STOLEN))
                {
                    ERRCHECK(result);
                }

                statusBar.Text = "Time " + (ms /1000 / 60) + ":" + (ms / 1000 % 60) + ":" + (ms / 10 % 100) + "/" + (lenms / 1000 / 60) + ":" + (lenms / 1000 % 60) + ":" + (lenms / 10 % 100) + (paused ? " Paused " : playing ? " Playing" : " Stopped");
            }

            if (system != null)
            {
                system.update();
            }
        }

        private void ERRCHECK(FMOD.RESULT result)
        {
            if (result != FMOD.RESULT.OK)
            {
                timer.Stop();
                MessageBox.Show("FMOD error! " + result + " - " + FMOD.Error.String(result));
                Environment.Exit(-1);
            }
        }

        private static float  t1 = 0, t2 = 0;        // time
        private static float  v1 = 0, v2 = 0;        // velocity

        private static FMOD.RESULT PCMREADCALLBACK(IntPtr soundraw, IntPtr data, uint datalen)
        {
            unsafe
            {
                uint count;
     
                short *stereo16bitbuffer = (short *)data.ToPointer();

                for (count=0; count < (datalen>>2); count++)        // >>2 = 16bit stereo (4 bytes per sample)
                {
                    *stereo16bitbuffer++ = (short)(Math.Sin(t1) * 32767.0f);    // left channel
                    *stereo16bitbuffer++ = (short)(Math.Sin(t2) * 32767.0f);    // right channel

                    t1 += 0.01f   + v1;
                    t2 += 0.0142f + v2;
                    v1 += (float)(Math.Sin(t1) * 0.002f);
                    v2 += (float)(Math.Sin(t2) * 0.002f);
                }
            }
            return FMOD.RESULT.OK;
        }

        private static FMOD.RESULT PCMSETPOSCALLBACK(IntPtr soundraw, int subsound, uint pcmoffset, FMOD.TIMEUNIT postype)
        {
            /*
                This is useful if the user calls Sound::setTime or Sound::setPosition and you want to seek your data accordingly.
            */

            return FMOD.RESULT.OK;
        }
	}
}
