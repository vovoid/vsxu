/*===============================================================================================
 Multi Speaker Output Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2008.

 This example shows how to play sounds in multiple speakers, and also how to even assign 
 sound subchannels, such as those in a stereo sound to different individual speakers.
===============================================================================================*/

using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

namespace multispeakeroutput
{
	public class MultiSpeakerOutput : System.Windows.Forms.Form
	{
        private FMOD.System  system  = null;
        private FMOD.Sound   sound1  = null, sound2 = null;
        private FMOD.Channel channel = null;

        private System.Windows.Forms.Label label;
        private System.Windows.Forms.Button exit_button;
        private System.Windows.Forms.StatusBar statusBar;
        private System.Windows.Forms.Timer timer;
        private System.Windows.Forms.Button frontLeft;
        private System.Windows.Forms.Button center;
        private System.Windows.Forms.Button frontRight;
        private System.Windows.Forms.Button rearLeft;
        private System.Windows.Forms.Button rearRight;
        private System.Windows.Forms.Button sideLeft;
        private System.Windows.Forms.Button sideRight;
        private System.Windows.Forms.Button stereoFront;
        private System.Windows.Forms.Button stereoSwapped;
        private System.Windows.Forms.Button rightStereo;
        private System.ComponentModel.IContainer components;

		public MultiSpeakerOutput()
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
            this.exit_button = new System.Windows.Forms.Button();
            this.statusBar = new System.Windows.Forms.StatusBar();
            this.timer = new System.Windows.Forms.Timer(this.components);
            this.frontLeft = new System.Windows.Forms.Button();
            this.center = new System.Windows.Forms.Button();
            this.frontRight = new System.Windows.Forms.Button();
            this.rearLeft = new System.Windows.Forms.Button();
            this.rearRight = new System.Windows.Forms.Button();
            this.sideLeft = new System.Windows.Forms.Button();
            this.sideRight = new System.Windows.Forms.Button();
            this.stereoFront = new System.Windows.Forms.Button();
            this.stereoSwapped = new System.Windows.Forms.Button();
            this.rightStereo = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // label
            // 
            this.label.Location = new System.Drawing.Point(14, 16);
            this.label.Name = "label";
            this.label.Size = new System.Drawing.Size(264, 32);
            this.label.TabIndex = 10;
            this.label.Text = "Copyright (c) Firelight Technologies 2004-2008";
            this.label.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // exit_button
            // 
            this.exit_button.Location = new System.Drawing.Point(112, 384);
            this.exit_button.Name = "exit_button";
            this.exit_button.Size = new System.Drawing.Size(72, 24);
            this.exit_button.TabIndex = 17;
            this.exit_button.Text = "Exit";
            this.exit_button.Click += new System.EventHandler(this.exit_button_Click);
            // 
            // statusBar
            // 
            this.statusBar.Location = new System.Drawing.Point(0, 411);
            this.statusBar.Name = "statusBar";
            this.statusBar.Size = new System.Drawing.Size(292, 24);
            this.statusBar.TabIndex = 20;
            // 
            // timer
            // 
            this.timer.Enabled = true;
            this.timer.Interval = 10;
            this.timer.Tick += new System.EventHandler(this.timer_Tick);
            // 
            // frontLeft
            // 
            this.frontLeft.Location = new System.Drawing.Point(8, 64);
            this.frontLeft.Name = "frontLeft";
            this.frontLeft.Size = new System.Drawing.Size(88, 56);
            this.frontLeft.TabIndex = 21;
            this.frontLeft.Text = "FRONT LEFT";
            this.frontLeft.Click += new System.EventHandler(this.frontLeft_Click);
            // 
            // center
            // 
            this.center.Location = new System.Drawing.Point(102, 64);
            this.center.Name = "center";
            this.center.Size = new System.Drawing.Size(88, 56);
            this.center.TabIndex = 22;
            this.center.Text = "CENTER";
            this.center.Click += new System.EventHandler(this.center_Click);
            // 
            // frontRight
            // 
            this.frontRight.Location = new System.Drawing.Point(200, 64);
            this.frontRight.Name = "frontRight";
            this.frontRight.Size = new System.Drawing.Size(88, 56);
            this.frontRight.TabIndex = 23;
            this.frontRight.Text = "FRONT RIGHT";
            this.frontRight.Click += new System.EventHandler(this.frontRight_Click);
            // 
            // rearLeft
            // 
            this.rearLeft.Location = new System.Drawing.Point(8, 200);
            this.rearLeft.Name = "rearLeft";
            this.rearLeft.Size = new System.Drawing.Size(88, 56);
            this.rearLeft.TabIndex = 24;
            this.rearLeft.Text = "REAR LEFT";
            this.rearLeft.Click += new System.EventHandler(this.rearLeft_Click);
            // 
            // rearRight
            // 
            this.rearRight.Location = new System.Drawing.Point(200, 200);
            this.rearRight.Name = "rearRight";
            this.rearRight.Size = new System.Drawing.Size(88, 56);
            this.rearRight.TabIndex = 25;
            this.rearRight.Text = "REAR RIGHT";
            this.rearRight.Click += new System.EventHandler(this.rearRight_Click);
            // 
            // sideLeft
            // 
            this.sideLeft.Location = new System.Drawing.Point(8, 136);
            this.sideLeft.Name = "sideLeft";
            this.sideLeft.Size = new System.Drawing.Size(88, 56);
            this.sideLeft.TabIndex = 26;
            this.sideLeft.Text = "SIDE LEFT";
            this.sideLeft.Click += new System.EventHandler(this.sideLeft_Click);
            // 
            // sideRight
            // 
            this.sideRight.Location = new System.Drawing.Point(200, 136);
            this.sideRight.Name = "sideRight";
            this.sideRight.Size = new System.Drawing.Size(88, 56);
            this.sideRight.TabIndex = 27;
            this.sideRight.Text = "SIDE RIGHT";
            this.sideRight.Click += new System.EventHandler(this.sideRight_Click);
            // 
            // stereoFront
            // 
            this.stereoFront.Location = new System.Drawing.Point(8, 264);
            this.stereoFront.Name = "stereoFront";
            this.stereoFront.Size = new System.Drawing.Size(280, 32);
            this.stereoFront.TabIndex = 28;
            this.stereoFront.Text = "Play stereo sound on front speakers";
            this.stereoFront.Click += new System.EventHandler(this.stereoFront_Click);
            // 
            // stereoSwapped
            // 
            this.stereoSwapped.Location = new System.Drawing.Point(8, 304);
            this.stereoSwapped.Name = "stereoSwapped";
            this.stereoSwapped.Size = new System.Drawing.Size(280, 32);
            this.stereoSwapped.TabIndex = 29;
            this.stereoSwapped.Text = "Play stereo sound on front speakers but with channels swapped";
            this.stereoSwapped.Click += new System.EventHandler(this.stereoSwapped_Click);
            // 
            // rightStereo
            // 
            this.rightStereo.Location = new System.Drawing.Point(8, 344);
            this.rightStereo.Name = "rightStereo";
            this.rightStereo.Size = new System.Drawing.Size(280, 32);
            this.rightStereo.TabIndex = 30;
            this.rightStereo.Text = "Play right part of a stereo sound on the CENTER speaker";
            this.rightStereo.Click += new System.EventHandler(this.rightStereo_Click);
            // 
            // MultiSpeakerOutput
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(292, 435);
            this.Controls.Add(this.rightStereo);
            this.Controls.Add(this.stereoSwapped);
            this.Controls.Add(this.stereoFront);
            this.Controls.Add(this.sideRight);
            this.Controls.Add(this.sideLeft);
            this.Controls.Add(this.rearRight);
            this.Controls.Add(this.rearLeft);
            this.Controls.Add(this.frontRight);
            this.Controls.Add(this.center);
            this.Controls.Add(this.frontLeft);
            this.Controls.Add(this.statusBar);
            this.Controls.Add(this.exit_button);
            this.Controls.Add(this.label);
            this.Name = "MultiSpeakerOutput";
            this.Text = "Multiple Speaker Output Example";
            this.Load += new System.EventHandler(this.MultiSpeakerOutput_Load);
            this.ResumeLayout(false);

        }
		#endregion

		[STAThread]
		static void Main() 
		{
			Application.Run(new MultiSpeakerOutput());
		}

        private void MultiSpeakerOutput_Load(object sender, System.EventArgs e)
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

            /* 
               Choose the highest mode possible.
               If the user doesn't have 7.1 speakers, then only the speakers they have will be audible. 
            */
            system.setSpeakerMode(FMOD.SPEAKERMODE._7POINT1);   
            ERRCHECK(result);

            result = system.init(32, FMOD.INITFLAG.NORMAL, (IntPtr)null);
            ERRCHECK(result);

            result = system.createSound("../../../../../examples/media/drumloop.wav", (FMOD.MODE.SOFTWARE | FMOD.MODE._2D), ref sound1);
            ERRCHECK(result);
            result = sound1.setMode(FMOD.MODE.LOOP_OFF);
            ERRCHECK(result);

            result = system.createSound("../../../../../examples/media/stereo.ogg", (FMOD.MODE.SOFTWARE | FMOD.MODE._2D), ref sound2);
            ERRCHECK(result);
        }

        private void frontLeft_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;

            result = system.playSound(FMOD.CHANNELINDEX.FREE, sound1, true, ref channel);
            ERRCHECK(result);

            result = channel.setSpeakerMix(1.0f, 0, 0, 0, 0, 0, 0, 0);
            ERRCHECK(result);

            result = channel.setPaused(false);
            ERRCHECK(result);
        }

        private void center_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;

            result = system.playSound(FMOD.CHANNELINDEX.FREE, sound1, true, ref channel);
            ERRCHECK(result);

            result = channel.setSpeakerMix(0, 0, 1.0f, 0, 0, 0, 0, 0);
            ERRCHECK(result);

            result = channel.setPaused(false);
            ERRCHECK(result);
        }

        private void frontRight_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;

            result = system.playSound(FMOD.CHANNELINDEX.FREE, sound1, true, ref channel);
            ERRCHECK(result);

            result = channel.setSpeakerMix(0, 1.0f, 0, 0, 0, 0, 0, 0);
            ERRCHECK(result);

            result = channel.setPaused(false);
            ERRCHECK(result);
        }

        private void sideLeft_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;

            result = system.playSound(FMOD.CHANNELINDEX.FREE, sound1, true, ref channel);
            ERRCHECK(result);

            result = channel.setSpeakerMix(0, 0, 0, 0, 0, 0, 1.0f, 0);
            ERRCHECK(result);

            result = channel.setPaused(false);
            ERRCHECK(result);
        }

        private void sideRight_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;

            result = system.playSound(FMOD.CHANNELINDEX.FREE, sound1, true, ref channel);
            ERRCHECK(result);

            result = channel.setSpeakerMix(0, 0, 0, 0, 0, 0, 0, 1.0f);
            ERRCHECK(result);

            result = channel.setPaused(false);
            ERRCHECK(result);
        }

        private void rearLeft_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;

            result = system.playSound(FMOD.CHANNELINDEX.FREE, sound1, true, ref channel);
            ERRCHECK(result);

            result = channel.setSpeakerMix(0, 0, 0, 0, 1.0f, 0, 0, 0);
            ERRCHECK(result);

            result = channel.setPaused(false);
            ERRCHECK(result);
        }

        private void rearRight_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;

            result = system.playSound(FMOD.CHANNELINDEX.FREE, sound1, true, ref channel);
            ERRCHECK(result);

            result = channel.setSpeakerMix(0, 0, 0, 0, 0, 1.0f, 0, 0);
            ERRCHECK(result);

            result = channel.setPaused(false);
            ERRCHECK(result);
        }

        private void stereoFront_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;

            result = system.playSound(FMOD.CHANNELINDEX.FREE, sound2, true, ref channel);
            ERRCHECK(result);

            /*
                By default a stereo sound would play in all right and all left speakers, so this forces it to just the front.
            */
            result = channel.setSpeakerMix(1.0f, 1.0f, 0, 0, 0, 0, 0, 0);
            ERRCHECK(result);

            result = channel.setPaused(false);
            ERRCHECK(result);
        }

        private void stereoSwapped_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;

            result = system.playSound(FMOD.CHANNELINDEX.FREE, sound2, true, ref channel);
            ERRCHECK(result);

            /*
                Clear out all speakers first.
            */
            result = channel.setSpeakerMix(0, 0, 0, 0, 0, 0, 0, 0);
            ERRCHECK(result);

            /*
                Put the left channel of the sound in the right speaker.
            */
            {
                float[] levels = { 0, 1.0f };    /* This array represents the source stereo sound.  l/r */

                result = channel.setSpeakerLevels(FMOD.SPEAKER.FRONT_LEFT, levels, 2);
                ERRCHECK(result);
            }
            /*
                 Put the right channel of the sound in the left speaker.
            */
            {
                float[] levels = { 1.0f, 0 };    /* This array represents the source stereo sound.  l/r */

                result = channel.setSpeakerLevels(FMOD.SPEAKER.FRONT_RIGHT, levels, 2);
                ERRCHECK(result);
            }

            result = channel.setPaused(false);
            ERRCHECK(result);
        }

        private void rightStereo_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;

            result = system.playSound(FMOD.CHANNELINDEX.FREE, sound2, true, ref channel);
            ERRCHECK(result);

            /*
                Clear out all speakers first.
            */
            result = channel.setSpeakerMix(0, 0, 0, 0, 0, 0, 0, 0);
            ERRCHECK(result);

            /*
                Put the right channel of the sound in the center speaker.
            */
            {
                float[] levels = { 0, 1.0f };    /* This array represents the source stereo sound.  l/r */

                result = channel.setSpeakerLevels(FMOD.SPEAKER.FRONT_CENTER, levels, 2);
                ERRCHECK(result);
            }

            result = channel.setPaused(false);
            ERRCHECK(result);
        }

        private void exit_button_Click(object sender, System.EventArgs e)
        {
            Application.Exit();
        }

        private void timer_Tick(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;
            uint         ms         = 0;
            uint         lenms      = 0;
            bool         playing    = false;
            bool         paused     = false;
            int          channelsplaying = 0;

            if (channel != null)
            {
                FMOD.Sound   currentsound    = null;

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
                
                system.getChannelsPlaying(ref channelsplaying);
            }

            statusBar.Text = "Time " + (ms / 1000 / 60) + ":" + (ms / 1000 % 60) + ":" + (ms / 10 % 100) + "/" + (lenms / 1000 / 60) + ":" + (lenms / 1000 % 60) + ":" + (lenms / 10 % 100) + " " + (paused ? "Paused " : playing ? "Playing" : "Stopped") + " Channels Playing " + channelsplaying;

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
