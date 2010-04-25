/*===============================================================================================
 DSP Effect per speaker Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2008.

 This example shows how to manipulate a DSP network and as an example, creates 2 dsp effects,
 and splits a single sound into 2 audio paths, which it then filters seperately.
 To only have each audio path come out of one speaker each, FMOD_DSP_setInputLevels is used just
 before the 2 branches merge back together again.

 For more speakers:
 1. Use FMOD_System_SetSpeakerMode or FMOD_System_SetOutputFormat.
 2. Create more effects, currently 2 for stereo (reverb and chorus), create one per speaker.
 3. Under the 'Now connect the 2 effects to channeldsp head.' section, connect the extra effects
    by duplicating the code more times.
 4. Filter each effect to each speaker by calling FMOD_DSP_SetInputLevels.  Expand the existing code
    by extending the level arrays from 2 to the number of speakers you require, and change the
    numlevels parameter in FMOD_DSP_SetInputLevels from 2 to the correct number accordingly.

===============================================================================================*/

using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

namespace dsp_effectperspeaker
{
	public class Form1 : System.Windows.Forms.Form
	{
        private FMOD.System     system    = null;
        private FMOD.Sound      sound     = null;
        private FMOD.Channel    channel   = null;
        private FMOD.DSP        dsplowpass = null, dspchorus = null;
        private FMOD.DSP        dsphead   = null, dspchannelmixer = null;
        private bool            reverbon  = false;
        private bool            choruson  = false;
		private FMOD.DSPConnection dsplowpassconnection = null;
		private FMOD.DSPConnection dspchorusconnection  = null;

        private System.Windows.Forms.Label label;
        private System.Windows.Forms.Timer timer;
        private System.Windows.Forms.Button exit_button;
        private System.Windows.Forms.StatusBar statusBar;
        private System.Windows.Forms.Button reverb;
        private System.Windows.Forms.Button chorus;
        private System.Windows.Forms.TrackBar trackBarPan;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.ComponentModel.IContainer components;

		public Form1()
		{
			InitializeComponent();
		}

		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
                FMOD.RESULT result;

                if (sound != null)
                {
                    result = sound.release();
                    ERRCHECK(result);
                }

                if (dsplowpass != null)
                {
                    result = dsplowpass.release();
                    ERRCHECK(result);
                }

                if (dspchorus != null)
                {
                    result = dspchorus.release();
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
            this.timer = new System.Windows.Forms.Timer(this.components);
            this.exit_button = new System.Windows.Forms.Button();
            this.statusBar = new System.Windows.Forms.StatusBar();
            this.reverb = new System.Windows.Forms.Button();
            this.chorus = new System.Windows.Forms.Button();
            this.trackBarPan = new System.Windows.Forms.TrackBar();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            ((System.ComponentModel.ISupportInitialize)(this.trackBarPan)).BeginInit();
            this.SuspendLayout();
            // 
            // label
            // 
            this.label.Location = new System.Drawing.Point(8, 8);
            this.label.Name = "label";
            this.label.Size = new System.Drawing.Size(264, 32);
            this.label.TabIndex = 10;
            this.label.Text = "Copyright (c) Firelight Technologies 2004-2008";
            this.label.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // timer
            // 
            this.timer.Enabled = true;
            this.timer.Interval = 10;
            this.timer.Tick += new System.EventHandler(this.timer_Tick);
            // 
            // exit_button
            // 
            this.exit_button.Location = new System.Drawing.Point(96, 176);
            this.exit_button.Name = "exit_button";
            this.exit_button.Size = new System.Drawing.Size(72, 24);
            this.exit_button.TabIndex = 20;
            this.exit_button.Text = "Exit";
            this.exit_button.Click += new System.EventHandler(this.exit_button_Click);
            // 
            // statusBar
            // 
            this.statusBar.Location = new System.Drawing.Point(0, 211);
            this.statusBar.Name = "statusBar";
            this.statusBar.Size = new System.Drawing.Size(264, 24);
            this.statusBar.TabIndex = 21;
            // 
            // reverb
            // 
            this.reverb.Location = new System.Drawing.Point(16, 48);
            this.reverb.Name = "reverb";
            this.reverb.Size = new System.Drawing.Size(104, 32);
            this.reverb.TabIndex = 22;
            this.reverb.Text = "Toggle reverb on left speaker only";
            this.reverb.Click += new System.EventHandler(this.reverb_Click);
            // 
            // chorus
            // 
            this.chorus.Location = new System.Drawing.Point(144, 48);
            this.chorus.Name = "chorus";
            this.chorus.Size = new System.Drawing.Size(104, 32);
            this.chorus.TabIndex = 23;
            this.chorus.Text = "Toggle chorus on right speaker only";
            this.chorus.Click += new System.EventHandler(this.chorus_Click);
            // 
            // trackBarPan
            // 
            this.trackBarPan.Location = new System.Drawing.Point(24, 112);
            this.trackBarPan.Maximum = 512;
            this.trackBarPan.Minimum = -512;
            this.trackBarPan.Name = "trackBarPan";
            this.trackBarPan.Size = new System.Drawing.Size(216, 45);
            this.trackBarPan.TabIndex = 24;
            this.trackBarPan.TickStyle = System.Windows.Forms.TickStyle.None;
            this.trackBarPan.Scroll += new System.EventHandler(this.trackBarPan_Scroll);
            // 
            // groupBox1
            // 
            this.groupBox1.Location = new System.Drawing.Point(8, 88);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(248, 80);
            this.groupBox1.TabIndex = 25;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Pan";
            // 
            // Form1
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(264, 235);
            this.Controls.Add(this.trackBarPan);
            this.Controls.Add(this.chorus);
            this.Controls.Add(this.reverb);
            this.Controls.Add(this.statusBar);
            this.Controls.Add(this.exit_button);
            this.Controls.Add(this.label);
            this.Controls.Add(this.groupBox1);
            this.Name = "Form1";
            this.Text = "DSP Effect per speaker example";
            this.Load += new System.EventHandler(this.Form1_Load);
            ((System.ComponentModel.ISupportInitialize)(this.trackBarPan)).EndInit();
            this.ResumeLayout(false);

        }
		#endregion

		[STAThread]
		static void Main() 
		{
			Application.Run(new Form1());
		}

        private void Form1_Load(object sender, System.EventArgs e)
        {
            uint            version = 0;
            FMOD.RESULT     result;
			FMOD.DSPConnection dspconnectiontemp = null;

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

            result = system.createSound("../../../../../examples/media/drumloop.wav", FMOD.MODE.SOFTWARE | FMOD.MODE.LOOP_NORMAL, ref sound);
            ERRCHECK(result);

            result = system.playSound(FMOD.CHANNELINDEX.FREE, sound, false, ref channel);
            ERRCHECK(result);

            /*
                Create the DSP effects.
            */  
            result = system.createDSPByType(FMOD.DSP_TYPE.LOWPASS, ref dsplowpass);
            ERRCHECK(result);

            result = system.createDSPByType(FMOD.DSP_TYPE.CHORUS, ref dspchorus);
            ERRCHECK(result);

            /*
                Connect up the DSP network
            */

            /*
                When a sound is played, a subnetwork is set up in the DSP network which looks like this.
                Wavetable is the drumloop sound, and it feeds its data from right to left.

                [DSPHEAD]<------------[DSPCHANNELMIXER]
            */  
            result = system.getDSPHead(ref dsphead);
            ERRCHECK(result);

            result = dsphead.getInput(0, ref dspchannelmixer, ref dspconnectiontemp);
            ERRCHECK(result);

            /*
                Now disconnect channeldsp head from wavetable to look like this.

                [DSPHEAD]             [DSPCHANNELMIXER]
            */
            result = dsphead.disconnectFrom(dspchannelmixer);
            ERRCHECK(result);

            /*
                Now connect the 2 effects to channeldsp head.

                          [DSPLOWPASS]
                         /           
                [DSPHEAD]             [DSPCHANNELMIXER]
                         \           
                          [DSPCHORUS]
            */
            result = dsphead.addInput(dsplowpass, ref dsplowpassconnection);
            ERRCHECK(result);
            result = dsphead.addInput(dspchorus, ref dspchorusconnection);
            ERRCHECK(result);
    
            /*
                Now connect the wavetable to the 2 effects

                          [DSPLOWPASS]
                         /           \
                [DSPHEAD]             [DSPCHANNELMIXER]
                         \           /
                          [DSPCHORUS]
            */
            result = dsplowpass.addInput(dspchannelmixer, ref dspconnectiontemp);
            ERRCHECK(result);
            result = dspchorus.addInput(dspchannelmixer, ref dspconnectiontemp);
            ERRCHECK(result);

            /*
                Now the drumloop will be twice as loud, because it is being split into 2, then recombined at the end.
                What we really want is to only feed the dspchannelmixer->dsplowpass through the left speaker, and 
                dspchannelmixer->dspchorus to the right speaker.
                We can do that simply by setting the pan, or speaker levels of the connections.

                          [DSPLOWPASS]
                         /1,0        \
                [DSPHEAD]             [DSPCHANNELMIXER]
                         \0,1        /
                          [DSPCHORUS]
            */
        {
            float[] leftinputon  = { 1.0f, 0.0f };
            float[] rightinputon = { 0.0f, 1.0f };
            float[] inputsoff    = { 0.0f, 0.0f };

            result = dsplowpassconnection.setLevels(FMOD.SPEAKER.FRONT_LEFT, leftinputon, 2);
            ERRCHECK(result);
            result = dsplowpassconnection.setLevels(FMOD.SPEAKER.FRONT_RIGHT, inputsoff, 2);
            ERRCHECK(result);

            result = dsplowpassconnection.setLevels(FMOD.SPEAKER.FRONT_LEFT, inputsoff, 2);
            ERRCHECK(result);
            result = dsplowpassconnection.setLevels(FMOD.SPEAKER.FRONT_RIGHT, rightinputon, 2);
            ERRCHECK(result);
        }

            result = dsplowpass.setBypass(true);
            result = dspchorus.setBypass(true);

            result = dsplowpass.setActive(true);
            result = dspchorus.setActive(true);

        }

        private void reverb_Click(object sender, System.EventArgs e)
        {
            dsplowpass.setBypass(reverbon);
            
            reverbon = !reverbon;
        }

        private void chorus_Click(object sender, System.EventArgs e)
        {
            dspchorus.setBypass(choruson);

            choruson = !choruson;
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
            int  channelsplaying = 0;
            float pan = 0.0f;

            if (channel != null)
            {
                channel.getPan(ref pan);
            }
            if (system != null)
            {
                system.getChannelsPlaying(ref channelsplaying);
                system.update();
            }

            statusBar.Text = "Channels Playing " + channelsplaying + " : Pan = " + pan;
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
