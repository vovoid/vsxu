/*===============================================================================================
 Custom DSP Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2006.

 This example shows how to add a user created DSP callback to process audio data.
 A read callback is generated at runtime, and can be added anywhere in the DSP network.

===============================================================================================*/

using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.Text;
using System.Runtime.InteropServices;

namespace dsp_custom
{
	public class Form1 : System.Windows.Forms.Form
	{
        private FMOD.System       system  = null;
        private FMOD.Sound        sound   = null;
        private FMOD.Channel      channel = null;
        private FMOD.DSP          mydsp   = null;

		private FMOD.DSPConnection dspconnectiontemp = null;

        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.Button activateButton;
        private System.Windows.Forms.Label label;
        private System.Windows.Forms.Button exit_button;
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
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.activateButton = new System.Windows.Forms.Button();
            this.label = new System.Windows.Forms.Label();
            this.exit_button = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // timer1
            // 
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // activateButton
            // 
            this.activateButton.Location = new System.Drawing.Point(72, 48);
            this.activateButton.Name = "activateButton";
            this.activateButton.Size = new System.Drawing.Size(120, 32);
            this.activateButton.TabIndex = 0;
            this.activateButton.Text = "Activate";
            this.activateButton.Click += new System.EventHandler(this.activateButton_Click);
            // 
            // label
            // 
            this.label.Location = new System.Drawing.Point(0, 8);
            this.label.Name = "label";
            this.label.Size = new System.Drawing.Size(264, 32);
            this.label.TabIndex = 11;
            this.label.Text = "Copyright (c) Firelight Technologies 2004-2008";
            this.label.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // exit_button
            // 
            this.exit_button.Location = new System.Drawing.Point(96, 88);
            this.exit_button.Name = "exit_button";
            this.exit_button.Size = new System.Drawing.Size(72, 24);
            this.exit_button.TabIndex = 21;
            this.exit_button.Text = "Exit";
            this.exit_button.Click += new System.EventHandler(this.exit_button_Click);
            // 
            // Form1
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(272, 115);
            this.Controls.Add(this.exit_button);
            this.Controls.Add(this.label);
            this.Controls.Add(this.activateButton);
            this.Name = "Form1";
            this.Text = "Custom DSP Example";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);

        }
		#endregion

		[STAThread]
		static void Main() 
		{
			Application.Run(new Form1());
		}

        private FMOD.DSP_DESCRIPTION  dspdesc = new FMOD.DSP_DESCRIPTION();
        private String                dspname = "My first DSP unit                 ";
        private FMOD.DSP_READCALLBACK dspreadcallback = new FMOD.DSP_READCALLBACK(READCALLBACK);
        private static IntPtr         name;
        private static FMOD.DSP       thisdsp = new FMOD.DSP();

        private static FMOD.RESULT READCALLBACK (ref FMOD.DSP_STATE dsp_state, IntPtr inbuf, IntPtr outbuf, uint length, int inchannels, int outchannels)
        {
            uint count, temp  = 0;
            int count2, temp1 = 0;
            IntPtr thisdspraw = dsp_state.instance; 

            thisdsp.setRaw(thisdspraw);

            /* 
                This redundant call just shows using the instance parameter of FMOD_DSP_STATE and using it to 
                call a DSP information function. 
            */
            thisdsp.getInfo(ref name, ref temp, ref temp1, ref temp1, ref temp1);

            /*
                This loop assumes inchannels = outchannels, which it will be if the DSP is created with '0' 
                as the number of channels in FMOD_DSP_DESCRIPTION.  
                Specifying an actual channel count will mean you have to take care of any number of channels coming in,
                but outputting the number of channels specified.  Generally it is best to keep the channel 
                count at 0 for maximum compatibility.
            */
            unsafe
            {
                float *inbuffer = (float *)inbuf.ToPointer();
                float *outbuffer = (float *)outbuf.ToPointer();

                for (count = 0; count < length; count++) 
                { 
                    /*
                        Feel free to unroll this.
                    */
                    for (count2 = 0; count2 < outchannels; count2++)
                    {
                        /* 
                            This DSP filter just halves the volume! 
                            Input is modified, and sent to output.
                        */
                        outbuffer[(count * outchannels) + count2] = inbuffer[(count * inchannels) + count2] * 0.2f;
                    }
                }
            }

            return FMOD.RESULT.OK;
        }


        private void Form1_Load(object sender, System.EventArgs e)
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

            result = system.createSound("../../../../../examples/media/drumloop.wav", FMOD.MODE.SOFTWARE | FMOD.MODE.LOOP_NORMAL, ref sound);
            ERRCHECK(result);

            result = system.playSound(FMOD.CHANNELINDEX.FREE, sound, false, ref channel);
            ERRCHECK(result);

            /*
                Create DSP unit
            */
            dspdesc.name     = dspname.ToCharArray(0, 32);
            dspdesc.channels = 0;
            dspdesc.read     = dspreadcallback;


            result = system.createDSP(ref dspdesc, ref mydsp);
            ERRCHECK(result);

            result = system.addDSP(mydsp, ref dspconnectiontemp);
            ERRCHECK(result);

            result = mydsp.setActive(true);
            ERRCHECK(result);

            result = mydsp.setBypass(true);
            ERRCHECK(result);

        }

        private void timer1_Tick(object sender, System.EventArgs e)
        {
            system.update();
        }

        private void activateButton_Click(object sender, System.EventArgs e)
        {
            bool bypass = false;

            mydsp.getBypass(ref bypass);
            mydsp.setBypass(!bypass);

            if (bypass)
            {
                activateButton.Text = "Deactivate";
            }
            else
            {
                activateButton.Text = "Activate";
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
                timer1.Stop();
                MessageBox.Show("FMOD error! " + result + " - " + FMOD.Error.String(result));
                Environment.Exit(-1);
            }
        }
	}
}
