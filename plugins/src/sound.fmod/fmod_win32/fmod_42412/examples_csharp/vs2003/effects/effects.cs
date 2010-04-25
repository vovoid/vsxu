/*===============================================================================================
 Effects Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2008.

 This example shows how to apply some of the built in software effects to sounds. 
 This example filters the global mix.  All software sounds played here would be filtered in the 
 same way.
 To filter per channel, and not have other channels affected, simply replace system->addDSP with
 channel->addDSP.
 Note in this example you don't have to add and remove units each time, you could simply add them 
 all at the start then use DSP::setActive to toggle them on and off.
===============================================================================================*/

using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

namespace effects
{
	public class effects : System.Windows.Forms.Form
	{
        private FMOD.System     system        = null;
        private FMOD.Sound      sound         = null;
        private FMOD.Channel    channel       = null;
        private FMOD.DSP        dsplowpass    = null;
        private FMOD.DSP        dsphighpass   = null;
        private FMOD.DSP        dspecho       = null;
        private FMOD.DSP        dspflange     = null;
        private FMOD.DSP        dspdistortion = null;
        private FMOD.DSP        dspchorus     = null;
        private FMOD.DSP        dspparameq    = null;

		private FMOD.DSPConnection dspconnectiontemp = null;

        private System.Windows.Forms.Label label;
        private System.Windows.Forms.Button pause_button;
        private System.Windows.Forms.Button exit_button;
        private System.Windows.Forms.CheckBox lowpass;
        private System.Windows.Forms.CheckBox highpass;
        private System.Windows.Forms.CheckBox echo;
        private System.Windows.Forms.CheckBox flange;
        private System.Windows.Forms.CheckBox distortion;
        private System.Windows.Forms.CheckBox chorus;
        private System.Windows.Forms.CheckBox parameq;
        private System.Windows.Forms.Timer timer;
        private System.ComponentModel.IContainer components;

		public effects()
		{
			InitializeComponent();
		}

		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
                FMOD.RESULT  result;

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
            this.pause_button = new System.Windows.Forms.Button();
            this.lowpass = new System.Windows.Forms.CheckBox();
            this.highpass = new System.Windows.Forms.CheckBox();
            this.echo = new System.Windows.Forms.CheckBox();
            this.flange = new System.Windows.Forms.CheckBox();
            this.distortion = new System.Windows.Forms.CheckBox();
            this.chorus = new System.Windows.Forms.CheckBox();
            this.parameq = new System.Windows.Forms.CheckBox();
            this.exit_button = new System.Windows.Forms.Button();
            this.timer = new System.Windows.Forms.Timer(this.components);
            this.SuspendLayout();
            // 
            // label
            // 
            this.label.Location = new System.Drawing.Point(0, 8);
            this.label.Name = "label";
            this.label.Size = new System.Drawing.Size(264, 32);
            this.label.TabIndex = 9;
            this.label.Text = "Copyright (c) Firelight Technologies 2004-2008";
            this.label.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // pause_button
            // 
            this.pause_button.Location = new System.Drawing.Point(32, 160);
            this.pause_button.Name = "pause_button";
            this.pause_button.Size = new System.Drawing.Size(192, 24);
            this.pause_button.TabIndex = 11;
            this.pause_button.Text = "Pause / Resume";
            this.pause_button.Click += new System.EventHandler(this.pause_button_Click);
            // 
            // lowpass
            // 
            this.lowpass.Location = new System.Drawing.Point(32, 56);
            this.lowpass.Name = "lowpass";
            this.lowpass.TabIndex = 12;
            this.lowpass.Text = "lowpass";
            this.lowpass.CheckedChanged += new System.EventHandler(this.lowpass_CheckedChanged);
            // 
            // highpass
            // 
            this.highpass.Location = new System.Drawing.Point(32, 80);
            this.highpass.Name = "highpass";
            this.highpass.TabIndex = 13;
            this.highpass.Text = "highpass";
            this.highpass.CheckedChanged += new System.EventHandler(this.highpass_CheckedChanged);
            // 
            // echo
            // 
            this.echo.Location = new System.Drawing.Point(32, 104);
            this.echo.Name = "echo";
            this.echo.TabIndex = 14;
            this.echo.Text = "echo";
            this.echo.CheckedChanged += new System.EventHandler(this.echo_CheckedChanged);
            // 
            // flange
            // 
            this.flange.Location = new System.Drawing.Point(32, 128);
            this.flange.Name = "flange";
            this.flange.TabIndex = 15;
            this.flange.Text = "flange";
            this.flange.CheckedChanged += new System.EventHandler(this.flange_CheckedChanged);
            // 
            // distortion
            // 
            this.distortion.Location = new System.Drawing.Point(144, 56);
            this.distortion.Name = "distortion";
            this.distortion.TabIndex = 16;
            this.distortion.Text = "distortion";
            this.distortion.CheckedChanged += new System.EventHandler(this.distortion_CheckedChanged);
            // 
            // chorus
            // 
            this.chorus.Location = new System.Drawing.Point(144, 80);
            this.chorus.Name = "chorus";
            this.chorus.TabIndex = 17;
            this.chorus.Text = "chorus";
            this.chorus.CheckedChanged += new System.EventHandler(this.chorus_CheckedChanged);
            // 
            // parameq
            // 
            this.parameq.Location = new System.Drawing.Point(144, 104);
            this.parameq.Name = "parameq";
            this.parameq.TabIndex = 18;
            this.parameq.Text = "parameq";
            this.parameq.CheckedChanged += new System.EventHandler(this.parameq_CheckedChanged);
            // 
            // exit_button
            // 
            this.exit_button.Location = new System.Drawing.Point(92, 192);
            this.exit_button.Name = "exit_button";
            this.exit_button.Size = new System.Drawing.Size(72, 24);
            this.exit_button.TabIndex = 19;
            this.exit_button.Text = "Exit";
            this.exit_button.Click += new System.EventHandler(this.exit_button_Click);
            // 
            // timer
            // 
            this.timer.Enabled = true;
            this.timer.Interval = 10;
            this.timer.Tick += new System.EventHandler(this.timer_Tick);
            // 
            // effects
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(256, 227);
            this.Controls.Add(this.exit_button);
            this.Controls.Add(this.parameq);
            this.Controls.Add(this.chorus);
            this.Controls.Add(this.distortion);
            this.Controls.Add(this.flange);
            this.Controls.Add(this.echo);
            this.Controls.Add(this.highpass);
            this.Controls.Add(this.lowpass);
            this.Controls.Add(this.pause_button);
            this.Controls.Add(this.label);
            this.Name = "effects";
            this.Text = "Effects Example";
            this.Load += new System.EventHandler(this.effects_Load);
            this.ResumeLayout(false);

        }
		#endregion


		[STAThread]
		static void Main() 
		{
			Application.Run(new effects());
		}

        private void effects_Load(object sender, System.EventArgs e)
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

            result = system.createSound("../../../../../examples/media/drumloop.wav", FMOD.MODE.SOFTWARE, ref sound);
            ERRCHECK(result);

            result = system.playSound(FMOD.CHANNELINDEX.FREE, sound, false, ref channel);
            ERRCHECK(result);

            /*
                Create some effects to play with.
            */
            result = system.createDSPByType(FMOD.DSP_TYPE.LOWPASS, ref dsplowpass);
            ERRCHECK(result);
            result = system.createDSPByType(FMOD.DSP_TYPE.HIGHPASS, ref dsphighpass);
            ERRCHECK(result);
            result = system.createDSPByType(FMOD.DSP_TYPE.ECHO, ref dspecho);
            ERRCHECK(result);
            result = system.createDSPByType(FMOD.DSP_TYPE.FLANGE, ref dspflange);
            ERRCHECK(result);
            result = system.createDSPByType(FMOD.DSP_TYPE.DISTORTION, ref dspdistortion);
            ERRCHECK(result);
            result = system.createDSPByType(FMOD.DSP_TYPE.CHORUS, ref dspchorus);
            ERRCHECK(result);
            result = system.createDSPByType(FMOD.DSP_TYPE.PARAMEQ, ref dspparameq);
            ERRCHECK(result);
        }

        private void lowpass_CheckedChanged(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;
            bool active = false;

            result = dsplowpass.getActive(ref active);
            ERRCHECK(result);

            if (active)
            {
                result = dsplowpass.remove();
                ERRCHECK(result);
            }
            else
            {
                result = system.addDSP(dsplowpass, ref dspconnectiontemp);
                ERRCHECK(result);
            }
        }

        private void highpass_CheckedChanged(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;
            bool active = false;

            result = dsphighpass.getActive(ref active);
            ERRCHECK(result);

            if (active)
            {
                result = dsphighpass.remove();
                ERRCHECK(result);
            }
            else
            {
                result = system.addDSP(dsphighpass,ref dspconnectiontemp);
                ERRCHECK(result);
            }
        }

        private void echo_CheckedChanged(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;
            bool active = false;

            result = dspecho.getActive(ref active);
            ERRCHECK(result);

            if (active)
            {
                result = dspecho.remove();
                ERRCHECK(result);
            }
            else
            {
                result = system.addDSP(dspecho, ref dspconnectiontemp);
                ERRCHECK(result);
            }
        }

        private void flange_CheckedChanged(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;
            bool active = false;

            result = dspflange.getActive(ref active);
            ERRCHECK(result);

            if (active)
            {
                result = dspflange.remove();
                ERRCHECK(result);
            }
            else
            {
                result = system.addDSP(dspflange, ref dspconnectiontemp);
                ERRCHECK(result);
            }
        }

        private void distortion_CheckedChanged(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;
            bool active = false;

            result = dspdistortion.getActive(ref active);
            ERRCHECK(result);

            if (active)
            {
                result = dspdistortion.remove();
                ERRCHECK(result);
            }
            else
            {
                result = system.addDSP(dspdistortion, ref dspconnectiontemp);
                ERRCHECK(result);
            }
        }

        private void chorus_CheckedChanged(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;
            bool active = false;

            result = dspchorus.getActive(ref active);
            ERRCHECK(result);

            if (active)
            {
                result = dspchorus.remove();
                ERRCHECK(result);
            }
            else
            {
                result = system.addDSP(dspchorus, ref dspconnectiontemp);
                ERRCHECK(result);
            }
        }

        private void parameq_CheckedChanged(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;
            bool active = false;

            result = dspparameq.getActive(ref active);
            ERRCHECK(result);

            if (active)
            {
                result = dspparameq.remove();
                ERRCHECK(result);
            }
            else
            {
                result = system.addDSP(dspparameq, ref dspconnectiontemp);
                ERRCHECK(result);
            }
        }

        private void pause_button_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;

            bool paused = false;

            result = channel.getPaused(ref paused);
            ERRCHECK(result);

            paused = !paused;

            result = channel.setPaused(paused);
            ERRCHECK(result);
        }
        
        private void timer_Tick(object sender, System.EventArgs e)
        {
            if (system != null)
            {
                system.update();
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
                timer.Stop();
                MessageBox.Show("FMOD error! " + result + " - " + FMOD.Error.String(result));
                Environment.Exit(-1);
            }
        }
	}
}
