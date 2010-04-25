/*===============================================================================================
 MultipleSoundCard Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2008.

 This example shows how to play sounds on 2 different sound cards from the same application.
 It creates 2 FMOD::System objects, selects a different sound device for each, then allows
 the user to play 1 sound on 1 soundcard and another sound on another soundcard.

 Note that sounds created on device A cannot be played on device B and vice versa.
 You will have to each sound separately for each sound card.  Device A might load the sound
 into its own memory so obviously device B wouldnt be able to play it.
===============================================================================================*/

using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.Text;

namespace multiplesoundcard
{
	public class MultipleSoundCard : System.Windows.Forms.Form
	{
        private FMOD.System     systemA = null, systemB = null;
        private FMOD.Sound      soundA = null, soundB = null;
        private FMOD.Channel    channelA = null, channelB = null;

        private System.Windows.Forms.Label label;
        private System.Windows.Forms.StatusBar statusBar;
        private System.Windows.Forms.Button exit_button;
        private System.Windows.Forms.Button playA;
        private System.Windows.Forms.ComboBox comboBox1;
        private System.Windows.Forms.ComboBox comboBox2;
        private System.Windows.Forms.Timer timer;
        private System.Windows.Forms.Button playB;
        private System.ComponentModel.IContainer components;

		public MultipleSoundCard()
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
                if (soundA != null)
                {
                    result = soundA.release();
                    ERRCHECK(result);
                }
                if (soundB != null)
                {
                    result = soundB.release();
                    ERRCHECK(result);
                }

                if (systemA != null)
                {
                    result = systemA.close();
                    ERRCHECK(result);
                }
                if (systemB != null)
                {
                    result = systemB.close();
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
            this.exit_button = new System.Windows.Forms.Button();
            this.playA = new System.Windows.Forms.Button();
            this.comboBox1 = new System.Windows.Forms.ComboBox();
            this.comboBox2 = new System.Windows.Forms.ComboBox();
            this.playB = new System.Windows.Forms.Button();
            this.timer = new System.Windows.Forms.Timer(this.components);
            this.SuspendLayout();
            // 
            // label
            // 
            this.label.Location = new System.Drawing.Point(0, 0);
            this.label.Name = "label";
            this.label.Size = new System.Drawing.Size(264, 32);
            this.label.TabIndex = 6;
            this.label.Text = "Copyright (c) Firelight Technologies 2004-2008";
            this.label.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // statusBar
            // 
            this.statusBar.Location = new System.Drawing.Point(0, 139);
            this.statusBar.Name = "statusBar";
            this.statusBar.Size = new System.Drawing.Size(272, 24);
            this.statusBar.TabIndex = 7;
            // 
            // exit_button
            // 
            this.exit_button.Location = new System.Drawing.Point(96, 112);
            this.exit_button.Name = "exit_button";
            this.exit_button.Size = new System.Drawing.Size(72, 24);
            this.exit_button.TabIndex = 8;
            this.exit_button.Text = "Exit";
            this.exit_button.Click += new System.EventHandler(this.exit_button_Click);
            // 
            // playA
            // 
            this.playA.Location = new System.Drawing.Point(176, 40);
            this.playA.Name = "playA";
            this.playA.Size = new System.Drawing.Size(80, 24);
            this.playA.TabIndex = 9;
            this.playA.Text = "Play";
            this.playA.Click += new System.EventHandler(this.playA_Click);
            // 
            // comboBox1
            // 
            this.comboBox1.Location = new System.Drawing.Point(8, 40);
            this.comboBox1.Name = "comboBox1";
            this.comboBox1.Size = new System.Drawing.Size(160, 21);
            this.comboBox1.TabIndex = 11;
            this.comboBox1.Text = "Select soundcard";
            this.comboBox1.SelectedIndexChanged += new System.EventHandler(this.comboBox1_SelectedIndexChanged);
            // 
            // comboBox2
            // 
            this.comboBox2.Location = new System.Drawing.Point(8, 80);
            this.comboBox2.Name = "comboBox2";
            this.comboBox2.Size = new System.Drawing.Size(160, 21);
            this.comboBox2.TabIndex = 13;
            this.comboBox2.Text = "Select soundcard";
            this.comboBox2.SelectedIndexChanged += new System.EventHandler(this.comboBox2_SelectedIndexChanged);
            // 
            // playB
            // 
            this.playB.Location = new System.Drawing.Point(176, 80);
            this.playB.Name = "playB";
            this.playB.Size = new System.Drawing.Size(80, 24);
            this.playB.TabIndex = 12;
            this.playB.Text = "Play";
            this.playB.Click += new System.EventHandler(this.playB_Click);
            // 
            // timer
            // 
            this.timer.Enabled = true;
            this.timer.Interval = 10;
            this.timer.Tick += new System.EventHandler(this.timer_Tick);
            // 
            // MultipleSoundCard
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(272, 163);
            this.Controls.Add(this.comboBox2);
            this.Controls.Add(this.playB);
            this.Controls.Add(this.comboBox1);
            this.Controls.Add(this.playA);
            this.Controls.Add(this.exit_button);
            this.Controls.Add(this.statusBar);
            this.Controls.Add(this.label);
            this.Name = "MultipleSoundCard";
            this.Text = "Multiple Sound Cards Example";
            this.Load += new System.EventHandler(this.MultipleSoundCard_Load);
            this.ResumeLayout(false);

        }
		#endregion

		[STAThread]
		static void Main() 
		{
			Application.Run(new MultipleSoundCard());
		}

        private void MultipleSoundCard_Load(object sender, System.EventArgs e)
        {
            FMOD.RESULT   result;
            uint          version = 0;
            int           numdrivers = 0;
            StringBuilder drivername = new StringBuilder(256);
			FMOD.GUID     guid = new FMOD.GUID();

            playA.Enabled = false;
            playB.Enabled = false;

            /*
                Create Sound Card A 
            */
            result = FMOD.Factory.System_Create(ref systemA);
            ERRCHECK(result);

            result = systemA.getVersion(ref version);
            ERRCHECK(result);
            if (version < FMOD.VERSION.number)
            {
                MessageBox.Show("Error!  You are using an old version of FMOD " + version.ToString("X") + ".  This program requires " + FMOD.VERSION.number.ToString("X") + ".");
                Application.Exit();
            }

            result = systemA.getNumDrivers(ref numdrivers);
            ERRCHECK(result);

            for (int count = 0; count < numdrivers; count++)
            {
                result = systemA.getDriverInfo(count, drivername, drivername.Capacity, ref guid);
                ERRCHECK(result);

                comboBox1.Items.Add(drivername.ToString());
            }

            /*
                Create Sound Card B
            */
            result = FMOD.Factory.System_Create(ref systemB);
            ERRCHECK(result);

            result = systemB.getVersion(ref version);
            ERRCHECK(result);
            if (version < FMOD.VERSION.number)
            {
                MessageBox.Show("Error!  You are using an old version of FMOD " + version.ToString("X") + ".  This program requires " + FMOD.VERSION.number.ToString("X") + ".");
                Application.Exit();
            }

            result = systemB.getNumDrivers(ref numdrivers);
            ERRCHECK(result);

            for (int count = 0; count < numdrivers; count++)
            {
                result = systemB.getDriverInfo(count, drivername, drivername.Capacity, ref guid);
                ERRCHECK(result);

                comboBox2.Items.Add(drivername.ToString());
            }
        }

        private void playA_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;

            result = systemA.playSound(FMOD.CHANNELINDEX.FREE, soundA, false, ref channelA);
            ERRCHECK(result);
        }

        private void playB_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;

            result = systemB.playSound(FMOD.CHANNELINDEX.FREE, soundB, false, ref channelB);
            ERRCHECK(result);
        }

        private void exit_button_Click(object sender, System.EventArgs e)
        {
            Application.Exit();

        }

        private void timer_Tick(object sender, System.EventArgs e)
        {
            int channelsplayingA = 0;
            int channelsplayingB = 0;

            if (channelA != null)
            {
                systemA.getChannelsPlaying(ref channelsplayingA);
            }
            if (channelB != null)
            {
                systemB.getChannelsPlaying(ref channelsplayingB);
            }

            statusBar.Text = "Channels Playing on A: " + channelsplayingA + "  Channels Playing on B: " + channelsplayingB;

            if (systemA != null)
            {
                systemA.update();
            }
            if (systemB != null)
            {
                systemB.update();
            }
        }

        private void comboBox1_SelectedIndexChanged(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;
            int         selected = comboBox1.SelectedIndex;

            result = systemA.setDriver(selected);
            ERRCHECK(result);
            
            result = systemA.init(32, FMOD.INITFLAG.NORMAL, (IntPtr)null);
            ERRCHECK(result);

            comboBox1.Enabled   = false;
            playA.Enabled       = true;

            /*
                Load a sample into soundcard 
            */
            result = systemA.createSound("../../../../../examples/media/drumloop.wav", FMOD.MODE.HARDWARE, ref soundA);
            ERRCHECK(result);
            result = soundA.setMode(FMOD.MODE.LOOP_OFF);
            ERRCHECK(result);
        }

        private void comboBox2_SelectedIndexChanged(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;
            int         selected = comboBox2.SelectedIndex;

            result = systemB.setDriver(selected);
            ERRCHECK(result);

            result = systemB.init(32, FMOD.INITFLAG.NORMAL, (IntPtr)null);
            ERRCHECK(result);

            comboBox2.Enabled   = false;
            playB.Enabled       = true;

            /*
                Load a sample into soundcard 
            */
            result = systemB.createSound("../../../../../examples/media/jaguar.wav", FMOD.MODE.HARDWARE, ref soundB);
            ERRCHECK(result);
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
