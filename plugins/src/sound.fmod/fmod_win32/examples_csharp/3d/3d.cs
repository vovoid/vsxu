/*===============================================================================================
 3d Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2008.

 This example shows how to basic 3d positioning
===============================================================================================*/

using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.Text;

namespace _3d
{
	public class threeD : System.Windows.Forms.Form
	{
        private const int   INTERFACE_UPDATETIME = 50;
        private const float DISTANCEFACTOR = 1.0f;          // Units per meter.  I.e feet would = 3.28.  centimeters would = 100.

        private FMOD.System  system   =  null;
        private FMOD.Sound   sound1   = null, sound2 = null, sound3 = null;
        private FMOD.Channel channel1 = null, channel2 = null, channel3 = null;
        private bool         listenerflag = true;

        private FMOD.VECTOR          lastpos = new FMOD.VECTOR();
        private FMOD.VECTOR          listenerpos = new FMOD.VECTOR();
        private static float         t = 0;

        private System.Windows.Forms.StatusBar statusBar;
        private System.Windows.Forms.Button exit_button;
        private System.Windows.Forms.Label label;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.Button button4;
        private System.Windows.Forms.Timer timer;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.TrackBar trackBarPosition;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.ComponentModel.IContainer components;

		public threeD()
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
            this.statusBar = new System.Windows.Forms.StatusBar();
            this.exit_button = new System.Windows.Forms.Button();
            this.label = new System.Windows.Forms.Label();
            this.button1 = new System.Windows.Forms.Button();
            this.button2 = new System.Windows.Forms.Button();
            this.button3 = new System.Windows.Forms.Button();
            this.button4 = new System.Windows.Forms.Button();
            this.timer = new System.Windows.Forms.Timer(this.components);
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.trackBarPosition = new System.Windows.Forms.TrackBar();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.trackBarPosition)).BeginInit();
            this.SuspendLayout();
            // 
            // statusBar
            // 
            this.statusBar.Location = new System.Drawing.Point(0, 291);
            this.statusBar.Name = "statusBar";
            this.statusBar.Size = new System.Drawing.Size(264, 24);
            this.statusBar.TabIndex = 18;
            // 
            // exit_button
            // 
            this.exit_button.Location = new System.Drawing.Point(96, 264);
            this.exit_button.Name = "exit_button";
            this.exit_button.Size = new System.Drawing.Size(72, 24);
            this.exit_button.TabIndex = 17;
            this.exit_button.Text = "Exit";
            this.exit_button.Click += new System.EventHandler(this.exit_button_Click);
            // 
            // label
            // 
            this.label.Location = new System.Drawing.Point(0, 0);
            this.label.Name = "label";
            this.label.Size = new System.Drawing.Size(264, 32);
            this.label.TabIndex = 16;
            this.label.Text = "Copyright (c) Firelight Technologies 2004-2008";
            this.label.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(16, 32);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(232, 32);
            this.button1.TabIndex = 19;
            this.button1.Text = "Pause/Unpause 16bit 3D sound at any time";
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(16, 72);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(232, 32);
            this.button2.TabIndex = 20;
            this.button2.Text = "Pause/Unpause 8bit 3D sound at any time";
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // button3
            // 
            this.button3.Location = new System.Drawing.Point(16, 112);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(232, 32);
            this.button3.TabIndex = 21;
            this.button3.Text = "Play 16bit STEREO 2D sound at any time";
            this.button3.Click += new System.EventHandler(this.button3_Click);
            // 
            // button4
            // 
            this.button4.Location = new System.Drawing.Point(16, 152);
            this.button4.Name = "button4";
            this.button4.Size = new System.Drawing.Size(232, 32);
            this.button4.TabIndex = 22;
            this.button4.Text = "Stop/Start listener automatic movement";
            this.button4.Click += new System.EventHandler(this.button4_Click);
            // 
            // timer
            // 
            this.timer.Enabled = true;
            this.timer.Interval = 49;
            this.timer.Tick += new System.EventHandler(this.timer_Tick);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.trackBarPosition);
            this.groupBox1.Location = new System.Drawing.Point(16, 192);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(232, 64);
            this.groupBox1.TabIndex = 23;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Listener Position";
            // 
            // trackBarPosition
            // 
            this.trackBarPosition.Location = new System.Drawing.Point(8, 16);
            this.trackBarPosition.Maximum = 35;
            this.trackBarPosition.Minimum = -35;
            this.trackBarPosition.Name = "trackBarPosition";
            this.trackBarPosition.Size = new System.Drawing.Size(216, 45);
            this.trackBarPosition.TabIndex = 0;
            this.trackBarPosition.TickStyle = System.Windows.Forms.TickStyle.None;
            this.trackBarPosition.Scroll += new System.EventHandler(this.trackBarPosition_Scroll);
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(80, 40);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(32, 16);
            this.label1.TabIndex = 1;
            this.label1.Text = "<1>";
            // 
            // label2
            // 
            this.label2.Location = new System.Drawing.Point(144, 40);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(24, 16);
            this.label2.TabIndex = 2;
            this.label2.Text = "<2>";
            // 
            // threeD
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(264, 315);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.button4);
            this.Controls.Add(this.button3);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.statusBar);
            this.Controls.Add(this.exit_button);
            this.Controls.Add(this.label);
            this.Name = "threeD";
            this.Text = "3D Example";
            this.Load += new System.EventHandler(this.threeD_Load);
            this.groupBox1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.trackBarPosition)).EndInit();
            this.ResumeLayout(false);

        }
		#endregion

		[STAThread]
		static void Main() 
		{
			Application.Run(new threeD());
		}

        private void threeD_Load(object sender, System.EventArgs e)
        {
            uint            version = 0;
            FMOD.RESULT     result;

            trackBarPosition.Enabled = false;

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

			FMOD.CAPS        caps        = FMOD.CAPS.NONE;
			FMOD.SPEAKERMODE speakermode = FMOD.SPEAKERMODE.STEREO;

			int	 minfrequency = 0, maxfrequency = 0;
			StringBuilder name = new StringBuilder();

			result = system.getDriverCaps(0, ref caps, ref minfrequency, ref maxfrequency, ref speakermode);
			ERRCHECK(result);

			result = system.setSpeakerMode(speakermode);                             /* Set the user selected speaker mode. */
			ERRCHECK(result);

			if ((caps & FMOD.CAPS.HARDWARE_EMULATED) == FMOD.CAPS.HARDWARE_EMULATED) /* The user has the 'Acceleration' slider set to off!  This is really bad for latency!. */
			{                                                                        /* You might want to warn the user about this. */ 
				result = system.setDSPBufferSize(1024, 10);	                     /* At 48khz, the latency between issuing an fmod command and hearing it will now be about 213ms. */ 
				ERRCHECK(result);
			}

			FMOD.GUID guid = new FMOD.GUID();

			result = system.getDriverInfo(0, name, 256, ref guid);
			ERRCHECK(result);

			if (name.ToString().IndexOf("SigmaTel") != -1)   /* Sigmatel sound devices crackle for some reason if the format is pcm 16bit.  pcm floating point output seems to solve it. */
			{
				result = system.setSoftwareFormat(48000, FMOD.SOUND_FORMAT.PCMFLOAT, 0,0, FMOD.DSP_RESAMPLER.LINEAR);
				ERRCHECK(result);
			}

            result = system.init(32, FMOD.INITFLAG.NORMAL, (IntPtr)null);
			if (result == FMOD.RESULT.ERR_OUTPUT_CREATEBUFFER)
			{
				result = system.setSpeakerMode(FMOD.SPEAKERMODE.STEREO);             /* Ok, the speaker mode selected isn't supported by this soundcard.  Switch it back to stereo... */ 
				ERRCHECK(result);

				result = system.init(32, FMOD.INITFLAG.NORMAL, (IntPtr)null);        /* Replace with whatever channel count and flags you use! */ 
				ERRCHECK(result);
			}

            /*
                Set the distance units. (meters/feet etc).
            */
            result = system.set3DSettings(1.0f, DISTANCEFACTOR, 1.0f);
            ERRCHECK(result);
            
            /*
                Load some sounds
            */
            result = system.createSound("../../../../examples/media/drumloop.wav", (FMOD.MODE.HARDWARE | FMOD.MODE._3D), ref sound1);
            ERRCHECK(result);
            result = sound1.set3DMinMaxDistance(2.0f * DISTANCEFACTOR, 10000.0f * DISTANCEFACTOR);
            ERRCHECK(result);
            result = sound1.setMode(FMOD.MODE.LOOP_NORMAL);
            ERRCHECK(result);

            result = system.createSound("../../../../examples/media/jaguar.wav", (FMOD.MODE.HARDWARE | FMOD.MODE._3D), ref sound2);
            ERRCHECK(result);
            result = sound2.set3DMinMaxDistance(2.0f * DISTANCEFACTOR, 10000.0f * DISTANCEFACTOR);
            ERRCHECK(result);
            result = sound2.setMode(FMOD.MODE.LOOP_NORMAL);
            ERRCHECK(result);

            result = system.createSound("../../../../examples/media/swish.wav", (FMOD.MODE.HARDWARE | FMOD.MODE._2D), ref sound3);
            ERRCHECK(result);

            /*
                Play sounds at certain positions
            */
            {
                FMOD.VECTOR pos1 = new FMOD.VECTOR();
                pos1.x = -10.0f * DISTANCEFACTOR; pos1.y = -0.0f; pos1.z = 0.0f;

                FMOD.VECTOR vel1 = new FMOD.VECTOR();
                vel1.x = 0.0f; vel1.y =  0.0f; vel1.z = 0.0f;

                result = system.playSound(FMOD.CHANNELINDEX.FREE, sound1, true, ref channel1);
                ERRCHECK(result);
                result = channel1.set3DAttributes(ref pos1, ref vel1);
                ERRCHECK(result);
                result = channel1.setPaused(false);
                ERRCHECK(result);
            }

            {
                FMOD.VECTOR pos2 = new FMOD.VECTOR();
                pos2.x = 15.0f * DISTANCEFACTOR; pos2.y = -0.0f; pos2.z = -0.0f;

                FMOD.VECTOR vel2 = new FMOD.VECTOR();
                vel2.x = 0.0f; vel2.y = 0.0f; vel2.z = 0.0f;

                result = system.playSound(FMOD.CHANNELINDEX.FREE, sound2, true, ref channel2);
                ERRCHECK(result);
                result = channel2.set3DAttributes(ref pos2, ref vel2);
                ERRCHECK(result);
                result = channel2.setPaused(false);
                ERRCHECK(result);
            }

            lastpos.x = 0.0f;
            lastpos.y = 0.0f;
            lastpos.z = 0.0f;

            listenerpos.x = 0.0f;
            listenerpos.y = 0.0f;
            listenerpos.z = -1.0f * DISTANCEFACTOR;
        }

        private void button1_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;

            bool paused = false;

            result = channel1.getPaused(ref paused);
            ERRCHECK(result);

            result = channel1.setPaused(!paused);
            ERRCHECK(result);
        }

        private void button2_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;

            bool paused = false;

            result = channel2.getPaused(ref paused);
            ERRCHECK(result);

            result = channel2.setPaused(!paused);
            ERRCHECK(result);
        }

        private void button3_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;

            result = system.playSound(FMOD.CHANNELINDEX.FREE, sound3, false, ref channel3);
            ERRCHECK(result);
        }

        private void button4_Click(object sender, System.EventArgs e)
        {
            trackBarPosition.Enabled = listenerflag;
            listenerflag = !listenerflag;
        }

        private void trackBarPosition_Scroll(object sender, System.EventArgs e)
        {
            listenerpos.x = trackBarPosition.Value * DISTANCEFACTOR;
        }

        private void exit_button_Click(object sender, System.EventArgs e)
        {
            Application.Exit();
        }

        private void timer_Tick(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;

            // ==========================================================================================
            // UPDATE THE LISTENER
            // ==========================================================================================

            FMOD.VECTOR forward  = new FMOD.VECTOR();
            forward.x = 0.0f; forward.y = 0.0f; forward.z = 1.0f;
            
            FMOD.VECTOR up = new FMOD.VECTOR();
            up.x = 0.0f; up.y = 1.0f; up.z = 0.0f;
            
            FMOD.VECTOR vel = new FMOD.VECTOR();

            if (listenerflag)
            {
                listenerpos.x = ((float)Math.Sin(t * 0.05f) * 33.0f * DISTANCEFACTOR); // left right pingpong
                trackBarPosition.Value = (int)listenerpos.x;
            }

            // ********* NOTE ******* READ NEXT COMMENT!!!!!
            // vel = how far we moved last FRAME (m/f), then time compensate it to SECONDS (m/s).
            vel.x = (listenerpos.x - lastpos.x) * (1000 / INTERFACE_UPDATETIME);
            vel.y = (listenerpos.y - lastpos.y) * (1000 / INTERFACE_UPDATETIME);
            vel.z = (listenerpos.z - lastpos.z) * (1000 / INTERFACE_UPDATETIME);

            // store pos for next time
            lastpos = listenerpos;

            result = system.set3DListenerAttributes(0, ref listenerpos, ref vel, ref forward, ref up);
            ERRCHECK(result);

            t += (30 * (1.0f / (float)INTERFACE_UPDATETIME));    // t is just a time value .. it increments in 30m/s steps in this example

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
