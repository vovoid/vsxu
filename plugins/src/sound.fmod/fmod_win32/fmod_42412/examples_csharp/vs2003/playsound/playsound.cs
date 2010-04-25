/*===============================================================================================
 PlaySound Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2008.

 This example shows how to simply load and play multiple sounds.  This is about the simplest
 use of FMOD.
 This makes FMOD decode the into memory when it loads.  If the sounds are big and possibly take
 up a lot of ram, then it would be better to use the FMOD_CREATESTREAM flag so that it is 
 streamed in realtime as it plays.
===============================================================================================*/

using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

namespace playsound
{
    public class PlaySound : System.Windows.Forms.Form
    {
        private FMOD.System     system  = null;
        private FMOD.Sound      sound1  = null, sound2 = null, sound3 = null;
        private FMOD.Channel    channel = null;
        private uint            ms      = 0;
        private uint            lenms   = 0;
        private bool            playing = false;
        private bool            paused  = false;
        private int             channelsplaying = 0;
 
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.Button exit_button;
        private System.Windows.Forms.Timer timer;
        private System.Windows.Forms.StatusBar statusBar;
        private System.Windows.Forms.Label label;
        private System.ComponentModel.IContainer components;

        public PlaySound()
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
                if (sound1 != null)
                {
                    result = sound1.release();
                    ERRCHECK(result);
                }
                if (sound2 != null)
                {
                    result = sound2.release();
                    ERRCHECK(result);
                }
                if (sound3 != null)
                {
                    result = sound3.release();
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
            this.button1 = new System.Windows.Forms.Button();
            this.button2 = new System.Windows.Forms.Button();
            this.button3 = new System.Windows.Forms.Button();
            this.exit_button = new System.Windows.Forms.Button();
            this.statusBar = new System.Windows.Forms.StatusBar();
            this.timer = new System.Windows.Forms.Timer(this.components);
            this.label = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(8, 40);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(232, 40);
            this.button1.TabIndex = 0;
            this.button1.Text = "Mono sound using hardware mixing";
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(8, 88);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(232, 40);
            this.button2.TabIndex = 1;
            this.button2.Text = "Mono sound using software mixing";
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // button3
            // 
            this.button3.Location = new System.Drawing.Point(8, 136);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(232, 40);
            this.button3.TabIndex = 2;
            this.button3.Text = "Stereo sound using hardware mixing";
            this.button3.Click += new System.EventHandler(this.button3_Click);
            // 
            // exit_button
            // 
            this.exit_button.Location = new System.Drawing.Point(88, 184);
            this.exit_button.Name = "exit_button";
            this.exit_button.Size = new System.Drawing.Size(72, 24);
            this.exit_button.TabIndex = 3;
            this.exit_button.Text = "Exit";
            this.exit_button.Click += new System.EventHandler(this.exit_button_Click);
            // 
            // statusBar
            // 
            this.statusBar.Location = new System.Drawing.Point(0, 211);
            this.statusBar.Name = "statusBar";
            this.statusBar.Size = new System.Drawing.Size(248, 24);
            this.statusBar.TabIndex = 4;
            // 
            // timer
            // 
            this.timer.Enabled = true;
            this.timer.Interval = 10;
            this.timer.Tick += new System.EventHandler(this.timer_Tick);
            // 
            // label
            // 
            this.label.Location = new System.Drawing.Point(0, 8);
            this.label.Name = "label";
            this.label.Size = new System.Drawing.Size(240, 32);
            this.label.TabIndex = 5;
            this.label.Text = "Copyright (c) Firelight Technologies 2004-2008";
            this.label.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // PlaySound
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(248, 235);
            this.Controls.Add(this.label);
            this.Controls.Add(this.statusBar);
            this.Controls.Add(this.exit_button);
            this.Controls.Add(this.button3);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.button1);
            this.Name = "PlaySound";
            this.Text = "Play Sound Example";
            this.Load += new System.EventHandler(this.PlaySound_Load);
            this.ResumeLayout(false);

        }
        #endregion

        [STAThread]
        static void Main() 
        {
            Application.Run(new PlaySound());
        }

        private void PlaySound_Load(object sender, System.EventArgs e)
        {
            uint            version = 0;
            FMOD.RESULT     result;

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

            result = system.createSound("../../../../../examples/media/drumloop.wav", FMOD.MODE.HARDWARE, ref sound1);
            ERRCHECK(result);

            result = sound1.setMode(FMOD.MODE.LOOP_OFF);
            ERRCHECK(result);

            result = system.createSound("../../../../../examples/media/jaguar.wav", FMOD.MODE.SOFTWARE, ref sound2);
            ERRCHECK(result);

            result = system.createSound("../../../../../examples/media/swish.wav", FMOD.MODE.HARDWARE, ref sound3);
            ERRCHECK(result);
        }

        private void button1_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT     result;

            result = system.playSound(FMOD.CHANNELINDEX.FREE, sound1, false, ref channel);
            ERRCHECK(result);
        }

        private void button2_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT     result;

            result = system.playSound(FMOD.CHANNELINDEX.FREE, sound2, false, ref channel);
            ERRCHECK(result);
        }

        private void button3_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT     result;

            result = system.playSound(FMOD.CHANNELINDEX.FREE, sound3, false, ref channel);
            ERRCHECK(result);
        }

        private void exit_button_Click(object sender, System.EventArgs e)
        {
            Application.Exit();
        }

        private void timer_Tick(object sender, System.EventArgs e)
        {   
            FMOD.RESULT     result;

            if (channel != null)
            {
                FMOD.Sound currentsound = null;

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
               
                channel.getCurrentSound(ref currentsound);
                if (currentsound != null)
                {
                    result = currentsound.getLength(ref lenms, FMOD.TIMEUNIT.MS);
                    if ((result != FMOD.RESULT.OK) && (result != FMOD.RESULT.ERR_INVALID_HANDLE) && (result != FMOD.RESULT.ERR_CHANNEL_STOLEN))
                    {
                        ERRCHECK(result);
                    }
                }
            }

            if (system != null)
            {
                system.getChannelsPlaying(ref channelsplaying);
            
                system.update();
            }

            statusBar.Text = "Time " + ms/1000/60 + ":" + ms/1000%60 + ":" + ms/10%100 + "/" + lenms/1000/60 + ":" + lenms/1000%60 + ":" + lenms/10%100
                + " : " + (paused ? "Paused " : playing ? "Playing" : "Stopped") + " : Channels Playing " + channelsplaying;
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
