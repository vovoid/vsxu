/*===============================================================================================
 ChannelGroups Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2008.

 This example shows how to put channels into channel groups, so that you can affect a group
 of channels at a time instead of just one channel at a time.
===============================================================================================*/
using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

namespace channelgroups
{
	public class Form1 : System.Windows.Forms.Form
	{
        private FMOD.System       system    = null;
        private FMOD.Sound[]      sound     = new FMOD.Sound[6];
        private FMOD.Channel[]    channel   = new FMOD.Channel[6];
        private FMOD.ChannelGroup groupA    = null, groupB = null, masterGroup = null;
        private bool              mutea = true, muteb = true, mutemaster = true;

        private System.Windows.Forms.Label label;
        private System.Windows.Forms.Timer timer;
        private System.Windows.Forms.Button exit_button;
        private System.Windows.Forms.StatusBar statusBar;
        private System.Windows.Forms.Button muteA;
        private System.Windows.Forms.Button muteB;
        private System.Windows.Forms.Button muteMaster;
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

                /*
                    Shut down
                */
                for (int count = 0; count < 6; count++)
                {
                    if (sound[count] != null)
                    {
                        result = sound[count].release();
                        ERRCHECK(result);
                    }
                }

                if (groupA != null)
                {
                    result = groupA.release();
                    ERRCHECK(result);
                }
                if (groupB != null)
                {
                    result = groupB.release();
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
            this.muteA = new System.Windows.Forms.Button();
            this.muteB = new System.Windows.Forms.Button();
            this.muteMaster = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // label
            // 
            this.label.Location = new System.Drawing.Point(16, 8);
            this.label.Name = "label";
            this.label.Size = new System.Drawing.Size(264, 32);
            this.label.TabIndex = 25;
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
            this.exit_button.Location = new System.Drawing.Point(112, 176);
            this.exit_button.Name = "exit_button";
            this.exit_button.Size = new System.Drawing.Size(72, 24);
            this.exit_button.TabIndex = 26;
            this.exit_button.Text = "Exit";
            this.exit_button.Click += new System.EventHandler(this.exit_button_Click);
            // 
            // statusBar
            // 
            this.statusBar.Location = new System.Drawing.Point(0, 211);
            this.statusBar.Name = "statusBar";
            this.statusBar.Size = new System.Drawing.Size(292, 24);
            this.statusBar.TabIndex = 27;
            // 
            // muteA
            // 
            this.muteA.Location = new System.Drawing.Point(56, 48);
            this.muteA.Name = "muteA";
            this.muteA.Size = new System.Drawing.Size(176, 32);
            this.muteA.TabIndex = 28;
            this.muteA.Text = "Mute/Unmute Group A";
            this.muteA.Click += new System.EventHandler(this.muteA_Click);
            // 
            // muteB
            // 
            this.muteB.Location = new System.Drawing.Point(56, 88);
            this.muteB.Name = "muteB";
            this.muteB.Size = new System.Drawing.Size(176, 32);
            this.muteB.TabIndex = 29;
            this.muteB.Text = "Mute/Unmute Group B";
            this.muteB.Click += new System.EventHandler(this.muteB_Click);
            // 
            // muteMaster
            // 
            this.muteMaster.Location = new System.Drawing.Point(56, 128);
            this.muteMaster.Name = "muteMaster";
            this.muteMaster.Size = new System.Drawing.Size(176, 32);
            this.muteMaster.TabIndex = 30;
            this.muteMaster.Text = "Mute/Unmute Group A and B (master group)";
            this.muteMaster.Click += new System.EventHandler(this.muteMaster_Click);
            // 
            // Form1
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(292, 235);
            this.Controls.Add(this.muteMaster);
            this.Controls.Add(this.muteB);
            this.Controls.Add(this.muteA);
            this.Controls.Add(this.statusBar);
            this.Controls.Add(this.exit_button);
            this.Controls.Add(this.label);
            this.Name = "Form1";
            this.Text = "ChannelGroups Example";
            this.Load += new System.EventHandler(this.Form1_Load);
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
            int             count = 0;
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
        
            result = system.createSound("../../../../../examples/media/drumloop.wav", FMOD.MODE.LOOP_NORMAL, ref sound[0]);
            ERRCHECK(result);
            result = system.createSound("../../../../../examples/media/jaguar.wav", FMOD.MODE.LOOP_NORMAL, ref sound[1]);
            ERRCHECK(result);
            result = system.createSound("../../../../../examples/media/swish.wav", FMOD.MODE.LOOP_NORMAL, ref sound[2]);
            ERRCHECK(result);
            result = system.createSound("../../../../../examples/media/c.ogg", FMOD.MODE.LOOP_NORMAL, ref sound[3]);
            ERRCHECK(result);
            result = system.createSound("../../../../../examples/media/d.ogg", FMOD.MODE.LOOP_NORMAL, ref sound[4]);
            ERRCHECK(result);
            result = system.createSound("../../../../../examples/media/e.ogg", FMOD.MODE.LOOP_NORMAL, ref sound[5]);
            ERRCHECK(result);

            result = system.createChannelGroup("Group A", ref groupA);
            ERRCHECK(result);

            result = system.createChannelGroup("Group B", ref groupB);
            ERRCHECK(result);

            result = system.getMasterChannelGroup(ref masterGroup);
            ERRCHECK(result);

            /*
                Instead of being independent, set the group A and B to be children of the master group.
            */
            result = masterGroup.addGroup(groupA);
            ERRCHECK(result);
 
            result = masterGroup.addGroup(groupB);
            ERRCHECK(result);

            /*
                Start all the sounds!
            */
            for (count = 0; count < 6; count++)
            {
                result = system.playSound(FMOD.CHANNELINDEX.FREE, sound[count], true, ref channel[count]);
                ERRCHECK(result);
                if (count < 3)
                {
                    result = channel[count].setChannelGroup(groupA);
                }
                else
                {
                    result = channel[count].setChannelGroup(groupB);
                }
                ERRCHECK(result);

                result = channel[count].setPaused(false);
                ERRCHECK(result);
            }
            
            /*
                Change the volume of each group, just because we can!  (And makes it less noise).
            */
            result = groupA.setVolume(0.5f);
            ERRCHECK(result);
            result = groupB.setVolume(0.5f);
            ERRCHECK(result);

        }

        private void muteA_Click(object sender, System.EventArgs e)
        {
            groupA.setMute(mutea);

            mutea = !mutea;
        }

        private void muteB_Click(object sender, System.EventArgs e)
        {
            groupB.setMute(muteb);

            muteb = !muteb;
        }
        
        private void muteMaster_Click(object sender, System.EventArgs e)
        {
            masterGroup.setMute(mutemaster);

            mutemaster = !mutemaster;
        }

        private void timer_Tick(object sender, System.EventArgs e)
        {
            int channelsplaying = 0;

            if (system != null)
            {
                system.getChannelsPlaying(ref channelsplaying);
                system.update();
            }

            statusBar.Text = "Channels Playing " + channelsplaying;
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
