/*===============================================================================================
 Submixing Example
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

namespace submixing
{
	public class Form1 : System.Windows.Forms.Form
	{
        private FMOD.System       system    = null;
        private FMOD.Sound[]      sound     = new FMOD.Sound[5];
        private FMOD.Channel[]    channel   = new FMOD.Channel[5];
        private FMOD.ChannelGroup groupA    = null, groupB = null, masterGroup = null;
        private FMOD.DSP          dspreverb = null, dspflange = null, dsplowpass = null;
        bool    mutea = true, muteb = true;
        bool    reverb = true, flange = true, lowpasson = true;
        
		private FMOD.DSPConnection dspconnectiontemp = null;

        private System.Windows.Forms.StatusBar statusBar;
        private System.Windows.Forms.Button exit_button;
        private System.Windows.Forms.Label label;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Button muteA;
        private System.Windows.Forms.Button flangeB;
        private System.Windows.Forms.Button muteB;
        private System.Windows.Forms.Button lowpass;
        private System.Windows.Forms.Button reverbA;
        private System.Windows.Forms.Timer timer;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.Label label18;
        private System.Windows.Forms.Label label19;
        private System.Windows.Forms.Label label20;
        private System.Windows.Forms.Label label21;
        private System.Windows.Forms.Label label22;
        private System.Windows.Forms.Label label23;
        private System.Windows.Forms.Label label24;
        private System.ComponentModel.IContainer components;

		public Form1()
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
                /*
                    Shut down
                */
                for (int count = 0; count < 5; count++)
                {
                    if (sound[count] != null)
                    {
                        result = sound[count].release();
                        ERRCHECK(result);
                    }
                }

                if (dspreverb != null)
                {
                    result = dspreverb.release();
                    ERRCHECK(result);
                }
                if (dspflange != null)
                {
                    result = dspflange.release();
                    ERRCHECK(result);
                }
                if (dsplowpass != null)
                {
                    result = dsplowpass.release();
                    ERRCHECK(result);
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
            this.statusBar = new System.Windows.Forms.StatusBar();
            this.exit_button = new System.Windows.Forms.Button();
            this.label = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.label12 = new System.Windows.Forms.Label();
            this.muteA = new System.Windows.Forms.Button();
            this.flangeB = new System.Windows.Forms.Button();
            this.muteB = new System.Windows.Forms.Button();
            this.lowpass = new System.Windows.Forms.Button();
            this.reverbA = new System.Windows.Forms.Button();
            this.timer = new System.Windows.Forms.Timer(this.components);
            this.label13 = new System.Windows.Forms.Label();
            this.label14 = new System.Windows.Forms.Label();
            this.label15 = new System.Windows.Forms.Label();
            this.label16 = new System.Windows.Forms.Label();
            this.label17 = new System.Windows.Forms.Label();
            this.label18 = new System.Windows.Forms.Label();
            this.label19 = new System.Windows.Forms.Label();
            this.label20 = new System.Windows.Forms.Label();
            this.label21 = new System.Windows.Forms.Label();
            this.label22 = new System.Windows.Forms.Label();
            this.label23 = new System.Windows.Forms.Label();
            this.label24 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // statusBar
            // 
            this.statusBar.Location = new System.Drawing.Point(0, 371);
            this.statusBar.Name = "statusBar";
            this.statusBar.Size = new System.Drawing.Size(496, 24);
            this.statusBar.TabIndex = 22;
            // 
            // exit_button
            // 
            this.exit_button.Location = new System.Drawing.Point(200, 336);
            this.exit_button.Name = "exit_button";
            this.exit_button.Size = new System.Drawing.Size(72, 24);
            this.exit_button.TabIndex = 23;
            this.exit_button.Text = "Exit";
            this.exit_button.Click += new System.EventHandler(this.exit_button_Click);
            // 
            // label
            // 
            this.label.Location = new System.Drawing.Point(14, 16);
            this.label.Name = "label";
            this.label.Size = new System.Drawing.Size(264, 32);
            this.label.TabIndex = 24;
            this.label.Text = "Copyright (c) Firelight Technologies 2004-2008";
            this.label.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(0, 0);
            this.label1.Name = "label1";
            this.label1.TabIndex = 42;
            // 
            // label2
            // 
            this.label2.Location = new System.Drawing.Point(88, 136);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(48, 16);
            this.label2.TabIndex = 26;
            this.label2.Text = "lowpass";
            // 
            // label3
            // 
            this.label3.Location = new System.Drawing.Point(152, 136);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(80, 16);
            this.label3.TabIndex = 27;
            this.label3.Text = "mastergroup";

            // 
            // label4
            // 
            this.label4.Location = new System.Drawing.Point(256, 176);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(64, 16);
            this.label4.TabIndex = 28;
            this.label4.Text = "flange";
            // 
            // label5
            // 
            this.label5.Location = new System.Drawing.Point(408, 208);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(64, 16);
            this.label5.TabIndex = 29;
            this.label5.Text = "d.ogg";
            // 
            // label6
            // 
            this.label6.Location = new System.Drawing.Point(256, 104);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(64, 16);
            this.label6.TabIndex = 30;
            this.label6.Text = "reverb";
            // 
            // label7
            // 
            this.label7.Location = new System.Drawing.Point(408, 48);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(88, 16);
            this.label7.TabIndex = 31;
            this.label7.Text = "drumloop.wav";
            // 
            // label8
            // 
            this.label8.Location = new System.Drawing.Point(408, 168);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(64, 16);
            this.label8.TabIndex = 32;
            this.label8.Text = "c.ogg";
            // 
            // label9
            // 
            this.label9.Location = new System.Drawing.Point(408, 96);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(64, 16);
            this.label9.TabIndex = 33;
            this.label9.Text = "jaguar.wav";
            // 
            // label10
            // 
            this.label10.Location = new System.Drawing.Point(328, 80);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(64, 16);
            this.label10.TabIndex = 34;
            this.label10.Text = "groupA";
            // 
            // label11
            // 
            this.label11.Location = new System.Drawing.Point(320, 208);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(64, 16);
            this.label11.TabIndex = 35;
            this.label11.Text = "groupB";
            // 
            // label12
            // 
            this.label12.Location = new System.Drawing.Point(408, 240);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(64, 16);
            this.label12.TabIndex = 36;
            this.label12.Text = "e.ogg";
            // 
            // muteA
            // 
            this.muteA.Location = new System.Drawing.Point(32, 224);
            this.muteA.Name = "muteA";
            this.muteA.Size = new System.Drawing.Size(96, 32);
            this.muteA.TabIndex = 37;
            this.muteA.Text = "Mute/Unmute group A";
            this.muteA.Click += new System.EventHandler(this.muteA_Click);
            // 
            // flangeB
            // 
            this.flangeB.Location = new System.Drawing.Point(144, 264);
            this.flangeB.Name = "flangeB";
            this.flangeB.Size = new System.Drawing.Size(96, 32);
            this.flangeB.TabIndex = 38;
            this.flangeB.Text = "Flange on group B";
            this.flangeB.Click += new System.EventHandler(this.flangeB_Click);
            // 
            // muteB
            // 
            this.muteB.Location = new System.Drawing.Point(144, 224);
            this.muteB.Name = "muteB";
            this.muteB.Size = new System.Drawing.Size(96, 32);
            this.muteB.TabIndex = 39;
            this.muteB.Text = "Mute/Unmute group B";
            this.muteB.Click += new System.EventHandler(this.muteB_Click);
            // 
            // lowpass
            // 
            this.lowpass.Location = new System.Drawing.Point(32, 304);
            this.lowpass.Name = "lowpass";
            this.lowpass.Size = new System.Drawing.Size(96, 48);
            this.lowpass.TabIndex = 40;
            this.lowpass.Text = "Lowpass on master group (everything)";
            this.lowpass.Click += new System.EventHandler(this.lowpass_Click);
            // 
            // reverbA
            // 
            this.reverbA.Location = new System.Drawing.Point(32, 264);
            this.reverbA.Name = "reverbA";
            this.reverbA.Size = new System.Drawing.Size(96, 32);
            this.reverbA.TabIndex = 41;
            this.reverbA.Text = "Reverb on group A";
            this.reverbA.Click += new System.EventHandler(this.reverbA_Click);
            // 
            // timer
            // 
            this.timer.Enabled = true;
            this.timer.Interval = 10;
            this.timer.Tick += new System.EventHandler(this.timer_Tick);
            // 
            // label13
            // 
            this.label13.Location = new System.Drawing.Point(8, 136);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(64, 16);
            this.label13.TabIndex = 43;
            this.label13.Text = "soundcard";
            // 
            // label14
            // 
            this.label14.Location = new System.Drawing.Point(232, 120);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(16, 16);
            this.label14.TabIndex = 44;
            this.label14.Text = "/";
            // 
            // label15
            // 
            this.label15.Location = new System.Drawing.Point(304, 96);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(16, 16);
            this.label15.TabIndex = 45;
            this.label15.Text = "/";
            // 
            // label16
            // 
            this.label16.Location = new System.Drawing.Point(384, 64);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(16, 16);
            this.label16.TabIndex = 46;
            this.label16.Text = "/";
            // 
            // label17
            // 
            this.label17.Location = new System.Drawing.Point(384, 192);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(16, 16);
            this.label17.TabIndex = 47;
            this.label17.Text = "/";
            // 
            // label18
            // 
            this.label18.Location = new System.Drawing.Point(232, 152);
            this.label18.Name = "label18";
            this.label18.Size = new System.Drawing.Size(16, 16);
            this.label18.TabIndex = 48;
            this.label18.Text = "\\";
            // 
            // label19
            // 
            this.label19.Location = new System.Drawing.Point(304, 192);
            this.label19.Name = "label19";
            this.label19.Size = new System.Drawing.Size(16, 16);
            this.label19.TabIndex = 49;
            this.label19.Text = "\\";
            // 
            // label20
            // 
            this.label20.Location = new System.Drawing.Point(384, 232);
            this.label20.Name = "label20";
            this.label20.Size = new System.Drawing.Size(16, 16);
            this.label20.TabIndex = 50;
            this.label20.Text = "\\";
            // 
            // label21
            // 
            this.label21.Location = new System.Drawing.Point(72, 136);
            this.label21.Name = "label21";
            this.label21.Size = new System.Drawing.Size(16, 16);
            this.label21.TabIndex = 51;
            this.label21.Text = "-";
            // 
            // label22
            // 
            this.label22.Location = new System.Drawing.Point(136, 136);
            this.label22.Name = "label22";
            this.label22.Size = new System.Drawing.Size(16, 16);
            this.label22.TabIndex = 52;
            this.label22.Text = "-";
            // 
            // label23
            // 
            this.label23.Location = new System.Drawing.Point(376, 208);
            this.label23.Name = "label23";
            this.label23.Size = new System.Drawing.Size(16, 16);
            this.label23.TabIndex = 53;
            this.label23.Text = "-";
            // 
            // label24
            // 
            this.label24.Location = new System.Drawing.Point(384, 88);
            this.label24.Name = "label24";
            this.label24.Size = new System.Drawing.Size(16, 16);
            this.label24.TabIndex = 54;
            this.label24.Text = "\\";
            // 
            // Form1
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(496, 395);
            this.Controls.Add(this.label24);
            this.Controls.Add(this.label23);
            this.Controls.Add(this.label22);
            this.Controls.Add(this.label21);
            this.Controls.Add(this.label20);
            this.Controls.Add(this.label19);
            this.Controls.Add(this.label18);
            this.Controls.Add(this.label17);
            this.Controls.Add(this.label16);
            this.Controls.Add(this.label15);
            this.Controls.Add(this.label14);
            this.Controls.Add(this.label13);
            this.Controls.Add(this.reverbA);
            this.Controls.Add(this.lowpass);
            this.Controls.Add(this.muteB);
            this.Controls.Add(this.flangeB);
            this.Controls.Add(this.muteA);
            this.Controls.Add(this.label12);
            this.Controls.Add(this.label11);
            this.Controls.Add(this.label10);
            this.Controls.Add(this.label9);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.label);
            this.Controls.Add(this.exit_button);
            this.Controls.Add(this.statusBar);
            this.Name = "Form1";
            this.Text = "Sub-mixing example";
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
        
            result = system.createSound("../../../../../examples/media/drumloop.wav", FMOD.MODE.SOFTWARE | FMOD.MODE.LOOP_NORMAL, ref sound[0]);
            ERRCHECK(result);
            result = system.createSound("../../../../../examples/media/jaguar.wav", FMOD.MODE.SOFTWARE | FMOD.MODE.LOOP_NORMAL, ref sound[1]);
            ERRCHECK(result);
            result = system.createSound("../../../../../examples/media/c.ogg", FMOD.MODE.SOFTWARE | FMOD.MODE.LOOP_NORMAL, ref sound[2]);
            ERRCHECK(result);
            result = system.createSound("../../../../../examples/media/d.ogg", FMOD.MODE.SOFTWARE | FMOD.MODE.LOOP_NORMAL, ref sound[3]);
            ERRCHECK(result);
            result = system.createSound("../../../../../examples/media/e.ogg", FMOD.MODE.SOFTWARE | FMOD.MODE.LOOP_NORMAL, ref sound[4]);
            ERRCHECK(result);

            result = system.createChannelGroup("Group A", ref groupA);
            ERRCHECK(result);

            result = system.createChannelGroup("Group B", ref groupB);
            ERRCHECK(result);

            result = system.getMasterChannelGroup(ref masterGroup);
            ERRCHECK(result);

            result = masterGroup.addGroup(groupA);
            ERRCHECK(result);

            result = masterGroup.addGroup(groupB);
            ERRCHECK(result);

            /*
                Start all the sounds!
            */
            for (count = 0; count < 5; count++)
            {
                result = system.playSound(FMOD.CHANNELINDEX.FREE, sound[count], true, ref channel[count]);
                ERRCHECK(result);
                if (count < 2)
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
                Create the DSP effects we want to apply to our submixes.
            */  
            result = system.createDSPByType(FMOD.DSP_TYPE.REVERB, ref dspreverb);
            ERRCHECK(result);

            result = system.createDSPByType(FMOD.DSP_TYPE.FLANGE, ref dspflange);
            ERRCHECK(result);
            result = dspflange.setParameter((int)FMOD.DSP_FLANGE.RATE, 1.0f);
            ERRCHECK(result);

            result = system.createDSPByType(FMOD.DSP_TYPE.LOWPASS, ref dsplowpass);
            ERRCHECK(result);
            result = dsplowpass.setParameter((int)FMOD.DSP_LOWPASS.CUTOFF, 500.0f);
            ERRCHECK(result);
        }

        private void timer_Tick(object sender, System.EventArgs e)
        {
            int  channelsplaying = 0;

            if (system != null)
            {
                system.getChannelsPlaying(ref channelsplaying);
                system.update();
            }

            statusBar.Text = "Channels Playing " + channelsplaying;
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

        private void reverbA_Click(object sender, System.EventArgs e)
        {
            if (reverb)
            {
                groupA.addDSP(dspreverb, ref dspconnectiontemp);
            }
            else
            {
                dspreverb.remove();
            }

            reverb = !reverb;
        }

        private void flangeB_Click(object sender, System.EventArgs e)
        {
            if (flange)
            {
                groupB.addDSP(dspflange, ref dspconnectiontemp);
            }
            else
            {
                dspflange.remove();
            }

            flange = !flange;
        }

        private void lowpass_Click(object sender, System.EventArgs e)
        {
            if (lowpasson)
            {
                masterGroup.addDSP(dsplowpass, ref dspconnectiontemp);
            }
            else
            {
                dsplowpass.remove();
            }

            lowpasson = !lowpasson;
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
