/*===============================================================================================
 NetStream Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2008.

 This example shows how to play streaming audio from the internet
===============================================================================================*/

using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.Runtime.InteropServices;

namespace netstream
{
	public class NetStream : System.Windows.Forms.Form
	{
        private FMOD.System     system  = null;
        private FMOD.Sound      sound   = null;
        private FMOD.Channel    channel = null;
        private bool            soundcreated = false;

        private System.Windows.Forms.Label label;
        private System.Windows.Forms.StatusBar statusBar;
        private System.Windows.Forms.Timer timer;
        private System.Windows.Forms.TextBox textBox;
        private System.Windows.Forms.Button exit_button;
        private System.Windows.Forms.Button playButton;
        private System.Windows.Forms.Button pauseButton;
        private System.ComponentModel.IContainer components;

		public NetStream()
		{
			InitializeComponent();
		}

		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
                FMOD.RESULT result;

                /*
                    Shutdown 
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
            this.statusBar = new System.Windows.Forms.StatusBar();
            this.timer = new System.Windows.Forms.Timer(this.components);
            this.textBox = new System.Windows.Forms.TextBox();
            this.exit_button = new System.Windows.Forms.Button();
            this.playButton = new System.Windows.Forms.Button();
            this.pauseButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // label
            // 
            this.label.Location = new System.Drawing.Point(14, 16);
            this.label.Name = "label";
            this.label.Size = new System.Drawing.Size(264, 32);
            this.label.TabIndex = 7;
            this.label.Text = "Copyright (c) Firelight Technologies 2004-2008";
            this.label.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // statusBar
            // 
            this.statusBar.Location = new System.Drawing.Point(0, 155);
            this.statusBar.Name = "statusBar";
            this.statusBar.Size = new System.Drawing.Size(296, 24);
            this.statusBar.TabIndex = 16;
            // 
            // timer
            // 
            this.timer.Enabled = true;
            this.timer.Interval = 10;
            this.timer.Tick += new System.EventHandler(this.timer_Tick);
            // 
            // textBox
            // 
            this.textBox.Location = new System.Drawing.Point(8, 56);
            this.textBox.Name = "textBox";
            this.textBox.Size = new System.Drawing.Size(280, 20);
            this.textBox.TabIndex = 17;
            this.textBox.Text = "http://";
            // 
            // exit_button
            // 
            this.exit_button.Location = new System.Drawing.Point(110, 128);
            this.exit_button.Name = "exit_button";
            this.exit_button.Size = new System.Drawing.Size(72, 24);
            this.exit_button.TabIndex = 18;
            this.exit_button.Text = "Exit";
            this.exit_button.Click += new System.EventHandler(this.exit_button_Click);
            // 
            // playButton
            // 
            this.playButton.Location = new System.Drawing.Point(8, 88);
            this.playButton.Name = "playButton";
            this.playButton.Size = new System.Drawing.Size(136, 32);
            this.playButton.TabIndex = 19;
            this.playButton.Text = "Start";
            this.playButton.Click += new System.EventHandler(this.playButton_Click);
            // 
            // pauseButton
            // 
            this.pauseButton.Location = new System.Drawing.Point(152, 88);
            this.pauseButton.Name = "pauseButton";
            this.pauseButton.Size = new System.Drawing.Size(136, 32);
            this.pauseButton.TabIndex = 20;
            this.pauseButton.Text = "Pause/Resume";
            this.pauseButton.Click += new System.EventHandler(this.pauseButton_Click);
            // 
            // NetStream
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(296, 179);
            this.Controls.Add(this.pauseButton);
            this.Controls.Add(this.playButton);
            this.Controls.Add(this.exit_button);
            this.Controls.Add(this.textBox);
            this.Controls.Add(this.statusBar);
            this.Controls.Add(this.label);
            this.Name = "NetStream";
            this.Text = "Netstream Example";
            this.Load += new System.EventHandler(this.NetStream_Load);
            this.ResumeLayout(false);

        }
		#endregion

		[STAThread]
		static void Main() 
		{
			Application.Run(new NetStream());
		}

        private void NetStream_Load(object sender, System.EventArgs e)
        {
            uint        version = 0;
            FMOD.RESULT result;

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
            result = system.init(1, FMOD.INITFLAG.NORMAL, (IntPtr)null);
            ERRCHECK(result);

            /*
                Bump up the file buffer size a little bit for netstreams (to account for lag).  Decode buffer is left at default.
            */
            result = system.setStreamBufferSize(64*1024, FMOD.TIMEUNIT.RAWBYTES);
            ERRCHECK(result);
        }

        private void playButton_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;
            string      url;

            if (!soundcreated)
            {
                statusBar.Text = "Buffering...";

                url = textBox.Text;

                result = system.createSound(url, (FMOD.MODE.HARDWARE | FMOD.MODE._2D | FMOD.MODE.CREATESTREAM | FMOD.MODE.NONBLOCKING), ref sound);
                ERRCHECK(result);

                soundcreated = true;
            }
        }

        private void pauseButton_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;
            bool        paused = false;

            if (channel != null)
            {
                result = channel.getPaused(ref paused);
                ERRCHECK(result);
                result = channel.setPaused(!paused);
                ERRCHECK(result);
            }
        }

        private void exit_button_Click(object sender, System.EventArgs e)
        {
            Application.Exit();
        }

        private void timer_Tick(object sender, System.EventArgs e)
        {
            FMOD.RESULT     result;
            FMOD.OPENSTATE  openstate = 0;
            uint            percentbuffered = 0;
            bool            starving = false;

            if (soundcreated)
            {
                result = sound.getOpenState(ref openstate, ref percentbuffered, ref starving);
                ERRCHECK(result);

                if (openstate == FMOD.OPENSTATE.READY && channel == null)
                {
                    result = system.playSound(FMOD.CHANNELINDEX.FREE, sound, false, ref channel);
                    ERRCHECK(result);
                }
            }

            if (channel != null)
            {
                uint ms      = 0;
                bool playing = false;
                bool paused  = false;

                for (;;)
                {
                    FMOD.TAG tag = new FMOD.TAG();
                    if (sound.getTag(null, -1, ref tag) != FMOD.RESULT.OK)
                    {
                        break;
                    }
                    if (tag.datatype == FMOD.TAGDATATYPE.STRING)
                    {
                        textBox.Text = tag.name + " = " + Marshal.PtrToStringAnsi(tag.data) + " (" + tag.datalen + " bytes)";
                    }
                    else
                    {
                        break;
                    }
                }

                result = channel.getPaused(ref paused);
                ERRCHECK(result);
                result = channel.isPlaying(ref playing);
                ERRCHECK(result);
                result = channel.getPosition(ref ms, FMOD.TIMEUNIT.MS);
                ERRCHECK(result);

                statusBar.Text = "Time " + (ms /1000 / 60) + ":" + (ms / 1000 % 60) + ":" + (ms / 10 % 100) + (openstate == FMOD.OPENSTATE.BUFFERING ? " Buffering..." : (openstate == FMOD.OPENSTATE.CONNECTING ? " Connecting..." : (paused ? " Paused       " : (playing ? " Playing      " : " Stopped      ")))) + "(" + percentbuffered + "%)" + (starving ? " STARVING" : "        ");
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
	}
}
