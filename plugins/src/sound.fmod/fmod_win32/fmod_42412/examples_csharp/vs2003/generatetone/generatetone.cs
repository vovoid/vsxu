/*===============================================================================================
 GenerateTone Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2008.

 This example shows how simply play generated tones using FMOD::System::payDSP instead of 
 manually connecting and disconnecting DSP units.
===============================================================================================*/

using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

namespace generatetone
{
	public class GenerateTone : System.Windows.Forms.Form
	{
        private FMOD.System    system       = null;
        private FMOD.Channel   channel      = null;
        private FMOD.DSP       dsp          = null;

        private System.Windows.Forms.Button exit_button;
        private System.Windows.Forms.Button sineWave;
        private System.Windows.Forms.Button squareWave;
        private System.Windows.Forms.Button triangleWave;
        private System.Windows.Forms.Button sawWave;
        private System.Windows.Forms.Button whiteNoise;
        private System.Windows.Forms.Button stopChannel;
        private System.Windows.Forms.StatusBar statusBar;
        private System.Windows.Forms.Label label;
        private System.Windows.Forms.Timer timer;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.TrackBar trackBarVolume;
        private System.Windows.Forms.TrackBar trackBarFrequency;
        private System.Windows.Forms.TrackBar trackBarPan;
        private System.ComponentModel.IContainer components;

		public GenerateTone()
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
                if (dsp != null)
                {
                    result = dsp.release();
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
            this.sineWave = new System.Windows.Forms.Button();
            this.squareWave = new System.Windows.Forms.Button();
            this.triangleWave = new System.Windows.Forms.Button();
            this.sawWave = new System.Windows.Forms.Button();
            this.whiteNoise = new System.Windows.Forms.Button();
            this.stopChannel = new System.Windows.Forms.Button();
            this.exit_button = new System.Windows.Forms.Button();
            this.statusBar = new System.Windows.Forms.StatusBar();
            this.label = new System.Windows.Forms.Label();
            this.timer = new System.Windows.Forms.Timer(this.components);
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.trackBarVolume = new System.Windows.Forms.TrackBar();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.trackBarFrequency = new System.Windows.Forms.TrackBar();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.trackBarPan = new System.Windows.Forms.TrackBar();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.trackBarVolume)).BeginInit();
            this.groupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.trackBarFrequency)).BeginInit();
            this.groupBox3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.trackBarPan)).BeginInit();
            this.SuspendLayout();
            // 
            // sineWave
            // 
            this.sineWave.Location = new System.Drawing.Point(8, 32);
            this.sineWave.Name = "sineWave";
            this.sineWave.Size = new System.Drawing.Size(280, 32);
            this.sineWave.TabIndex = 0;
            this.sineWave.Text = "Play a sine wave";
            this.sineWave.Click += new System.EventHandler(this.sineWave_Click);
            // 
            // squareWave
            // 
            this.squareWave.Location = new System.Drawing.Point(8, 72);
            this.squareWave.Name = "squareWave";
            this.squareWave.Size = new System.Drawing.Size(280, 32);
            this.squareWave.TabIndex = 1;
            this.squareWave.Text = "Play a square wave";
            this.squareWave.Click += new System.EventHandler(this.squareWave_Click);
            // 
            // triangleWave
            // 
            this.triangleWave.Location = new System.Drawing.Point(8, 112);
            this.triangleWave.Name = "triangleWave";
            this.triangleWave.Size = new System.Drawing.Size(280, 32);
            this.triangleWave.TabIndex = 2;
            this.triangleWave.Text = "Play a triangle wave";
            this.triangleWave.Click += new System.EventHandler(this.triangleWave_Click);
            // 
            // sawWave
            // 
            this.sawWave.Location = new System.Drawing.Point(8, 152);
            this.sawWave.Name = "sawWave";
            this.sawWave.Size = new System.Drawing.Size(280, 32);
            this.sawWave.TabIndex = 3;
            this.sawWave.Text = "Play a saw wave";
            this.sawWave.Click += new System.EventHandler(this.sawWave_Click);
            // 
            // whiteNoise
            // 
            this.whiteNoise.Location = new System.Drawing.Point(8, 192);
            this.whiteNoise.Name = "whiteNoise";
            this.whiteNoise.Size = new System.Drawing.Size(280, 32);
            this.whiteNoise.TabIndex = 4;
            this.whiteNoise.Text = "Play a white noise";
            this.whiteNoise.Click += new System.EventHandler(this.whiteNoise_Click);
            // 
            // stopChannel
            // 
            this.stopChannel.Location = new System.Drawing.Point(8, 232);
            this.stopChannel.Name = "stopChannel";
            this.stopChannel.Size = new System.Drawing.Size(280, 32);
            this.stopChannel.TabIndex = 5;
            this.stopChannel.Text = "Stop channel";
            this.stopChannel.Click += new System.EventHandler(this.stopChannel_Click);
            // 
            // exit_button
            // 
            this.exit_button.Location = new System.Drawing.Point(112, 488);
            this.exit_button.Name = "exit_button";
            this.exit_button.Size = new System.Drawing.Size(72, 24);
            this.exit_button.TabIndex = 12;
            this.exit_button.Text = "Exit";
            this.exit_button.Click += new System.EventHandler(this.exit_button_Click);
            // 
            // statusBar
            // 
            this.statusBar.Location = new System.Drawing.Point(0, 515);
            this.statusBar.Name = "statusBar";
            this.statusBar.Size = new System.Drawing.Size(296, 24);
            this.statusBar.TabIndex = 13;
            // 
            // label
            // 
            this.label.Location = new System.Drawing.Point(0, 0);
            this.label.Name = "label";
            this.label.Size = new System.Drawing.Size(296, 32);
            this.label.TabIndex = 14;
            this.label.Text = "Copyright (c) Firelight Technologies 2004-2008";
            this.label.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // timer
            // 
            this.timer.Enabled = true;
            this.timer.Interval = 10;
            this.timer.Tick += new System.EventHandler(this.timer_Tick);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.trackBarVolume);
            this.groupBox1.Location = new System.Drawing.Point(8, 272);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(280, 64);
            this.groupBox1.TabIndex = 15;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Channel Volume";
            // 
            // trackBarVolume
            // 
            this.trackBarVolume.Location = new System.Drawing.Point(8, 16);
            this.trackBarVolume.Maximum = 1024;
            this.trackBarVolume.Name = "trackBarVolume";
            this.trackBarVolume.Size = new System.Drawing.Size(264, 45);
            this.trackBarVolume.TabIndex = 0;
            this.trackBarVolume.TickStyle = System.Windows.Forms.TickStyle.None;
            this.trackBarVolume.Scroll += new System.EventHandler(this.trackBarVolume_Scroll);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.trackBarFrequency);
            this.groupBox2.Location = new System.Drawing.Point(8, 344);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(280, 64);
            this.groupBox2.TabIndex = 16;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Channel Frequency";
            // 
            // trackBarFrequency
            // 
            this.trackBarFrequency.Location = new System.Drawing.Point(8, 16);
            this.trackBarFrequency.Maximum = 88200;
            this.trackBarFrequency.Name = "trackBarFrequency";
            this.trackBarFrequency.Size = new System.Drawing.Size(264, 45);
            this.trackBarFrequency.TabIndex = 1;
            this.trackBarFrequency.TickStyle = System.Windows.Forms.TickStyle.None;
            this.trackBarFrequency.Scroll += new System.EventHandler(this.trackBarFrequency_Scroll);
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.trackBarPan);
            this.groupBox3.Location = new System.Drawing.Point(8, 416);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(280, 64);
            this.groupBox3.TabIndex = 17;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Channel Pan";
            // 
            // trackBarPan
            // 
            this.trackBarPan.Location = new System.Drawing.Point(8, 16);
            this.trackBarPan.Maximum = 512;
            this.trackBarPan.Minimum = -512;
            this.trackBarPan.Name = "trackBarPan";
            this.trackBarPan.Size = new System.Drawing.Size(264, 45);
            this.trackBarPan.TabIndex = 1;
            this.trackBarPan.TickStyle = System.Windows.Forms.TickStyle.None;
            this.trackBarPan.Scroll += new System.EventHandler(this.trackBarPan_Scroll);
            // 
            // GenerateTone
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(296, 539);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.label);
            this.Controls.Add(this.statusBar);
            this.Controls.Add(this.exit_button);
            this.Controls.Add(this.stopChannel);
            this.Controls.Add(this.whiteNoise);
            this.Controls.Add(this.sawWave);
            this.Controls.Add(this.triangleWave);
            this.Controls.Add(this.squareWave);
            this.Controls.Add(this.sineWave);
            this.Name = "GenerateTone";
            this.Text = "Generate Tone Example";
            this.Load += new System.EventHandler(this.GenerateTone_Load);
            this.groupBox1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.trackBarVolume)).EndInit();
            this.groupBox2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.trackBarFrequency)).EndInit();
            this.groupBox3.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.trackBarPan)).EndInit();
            this.ResumeLayout(false);

        }
		#endregion

		[STAThread]
		static void Main() 
		{
			Application.Run(new GenerateTone());
		}

        private void GenerateTone_Load(object sender, System.EventArgs e)
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
        }

        private void sineWave_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;
            float pan = 0, frequency = 0;

            if (channel != null)
            {
                channel.stop();
            }

            if (dsp != null)
            {
                dsp.release();
            }

            /*
                Create an oscillator DSP unit for the tone.
            */
            result = system.createDSPByType(FMOD.DSP_TYPE.OSCILLATOR, ref dsp);
            ERRCHECK(result);
            result = dsp.setParameter((int)FMOD.DSP_OSCILLATOR.RATE, 440.0f);       /* musical note 'A' */
            ERRCHECK(result);

            result = system.playDSP(FMOD.CHANNELINDEX.REUSE, dsp, true, ref channel);
            ERRCHECK(result);
            channel.setVolume(0.5f);
            result = dsp.setParameter((int)FMOD.DSP_OSCILLATOR.TYPE, 0);
            ERRCHECK(result);
            channel.setPaused(false);

            channel.getFrequency(ref frequency);
            channel.getPan(ref pan);
            
            trackBarVolume.Value = 512;
            trackBarFrequency.Value = (int)frequency;
            trackBarPan.Value = (int)(pan * 512);
        }

        private void squareWave_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;
            float pan = 0, frequency = 0;

            if (channel != null)
            {
                channel.stop();
            }
            if (dsp != null)
            {
                dsp.release();
            }

            /*
                Create an oscillator DSP unit for the tone.
            */
            result = system.createDSPByType(FMOD.DSP_TYPE.OSCILLATOR, ref dsp);
            ERRCHECK(result);
            result = dsp.setParameter((int)FMOD.DSP_OSCILLATOR.RATE, 440.0f);       /* musical note 'A' */
            ERRCHECK(result);

            result = system.playDSP(FMOD.CHANNELINDEX.REUSE, dsp, true, ref channel);
            ERRCHECK(result);
            channel.setVolume(0.125f);
            result = dsp.setParameter((int)FMOD.DSP_OSCILLATOR.TYPE, 1);
            ERRCHECK(result);
            channel.setPaused(false);

            channel.getFrequency(ref frequency);
            channel.getPan(ref pan);
            
            trackBarVolume.Value = 128;
            trackBarFrequency.Value = (int)frequency;
            trackBarPan.Value = (int)(pan * 512);
        }

        private void triangleWave_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;
            float pan = 0, frequency = 0;

            if (channel != null)
            {
                channel.stop();
            }
            if (dsp != null)
            {
                dsp.release();
            }

            /*
                Create an oscillator DSP unit for the tone.
            */
            result = system.createDSPByType(FMOD.DSP_TYPE.OSCILLATOR, ref dsp);
            ERRCHECK(result);
            result = dsp.setParameter((int)FMOD.DSP_OSCILLATOR.RATE, 440.0f);       /* musical note 'A' */
            ERRCHECK(result);

            result = system.playDSP(FMOD.CHANNELINDEX.REUSE, dsp, true, ref channel);
            ERRCHECK(result);
            channel.setVolume(0.5f);
            result = dsp.setParameter((int)FMOD.DSP_OSCILLATOR.TYPE, 2);
            ERRCHECK(result);
            channel.setPaused(false);

            channel.getFrequency(ref frequency);
            channel.getPan(ref pan);
            
            trackBarVolume.Value = 512;
            trackBarFrequency.Value = (int)frequency;
            trackBarPan.Value = (int)(pan * 512);
        }

        private void sawWave_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;
            float pan = 0, frequency = 0;

            if (channel != null)
            {
                channel.stop();
            }
            if (dsp != null)
            {
                dsp.release();
            }

            /*
                Create an oscillator DSP unit for the tone.
            */
            result = system.createDSPByType(FMOD.DSP_TYPE.OSCILLATOR, ref dsp);
            ERRCHECK(result);
            result = dsp.setParameter((int)FMOD.DSP_OSCILLATOR.RATE, 440.0f);       /* musical note 'A' */
            ERRCHECK(result);

            result = system.playDSP(FMOD.CHANNELINDEX.REUSE, dsp, true, ref channel);
            ERRCHECK(result);
            channel.setVolume(0.125f);
            result = dsp.setParameter((int)FMOD.DSP_OSCILLATOR.TYPE, 4);
            ERRCHECK(result);
            channel.setPaused(false);
 
            channel.getFrequency(ref frequency);
            channel.getPan(ref pan);
            
            trackBarVolume.Value = 128;
            trackBarFrequency.Value = (int)frequency;
            trackBarPan.Value = (int)(pan * 512);
        }

        private void whiteNoise_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;
            float pan = 0, frequency = 0;

            if (channel != null)
            {
                channel.stop();
            }
            if (dsp != null)
            {
                dsp.release();
            }

            /*
                Create an oscillator DSP unit for the tone.
            */
            result = system.createDSPByType(FMOD.DSP_TYPE.OSCILLATOR, ref dsp);
            ERRCHECK(result);
            result = dsp.setParameter((int)FMOD.DSP_OSCILLATOR.RATE, 440.0f);       /* musical note 'A' */
            ERRCHECK(result);

            result = system.playDSP(FMOD.CHANNELINDEX.REUSE, dsp, true, ref channel);
            ERRCHECK(result);
            channel.setVolume(0.25f);
            result = dsp.setParameter((int)FMOD.DSP_OSCILLATOR.TYPE, 5);
            ERRCHECK(result);
            channel.setPaused(false);

            channel.getFrequency(ref frequency);
            channel.getPan(ref pan);
            
            trackBarVolume.Value = 256;
            trackBarFrequency.Value = (int)frequency;
            trackBarPan.Value = (int)(pan * 512);
        }

        private void stopChannel_Click(object sender, System.EventArgs e)
        {
            if (channel != null)
            {
                channel.stop();
            }
        }
        
        private void trackBarVolume_Scroll(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;

            if (channel != null)
            {
                float volume = (float)(trackBarVolume.Value) / 1024.0f;

                result = channel.setVolume(volume);
                ERRCHECK(result);
            }
        }

        private void trackBarFrequency_Scroll(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;

            if (channel != null)
            {
                result = channel.setFrequency((float)trackBarFrequency.Value);
                ERRCHECK(result);
            }
        }

        private void trackBarPan_Scroll(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;

            if (channel != null)
            {
                float pan = (float)(trackBarPan.Value) / 512.0f;

                result = channel.setPan(pan);
            }
        }

        private void exit_button_Click(object sender, System.EventArgs e)
        {
            Application.Exit();
        }

        private void timer_Tick(object sender, System.EventArgs e)
        {
            float frequency = 0, volume = 0, pan = 0;
            bool playing = false;

            if (channel != null)
            {
                channel.getFrequency(ref frequency);
                channel.getVolume(ref volume);
                channel.getPan(ref pan);
                channel.isPlaying(ref playing);
            }

            statusBar.Text = "Channel " + (playing ? "playing" : "stopped") + " : Frequency " + frequency + " Volume " + volume + " Pan " + pan;

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
