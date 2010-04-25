/*===============================================================================================
  Spectrum Example
  Copyright (c), Firelight Technologies Pty, Ltd 2004-2008.
 
  This example shows how to draw a spectrum as well as an oscilliscope of the wave data
===============================================================================================*/

using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

namespace spectrum
{
	public class Form1 : System.Windows.Forms.Form
	{
        private const int GRAPHICWINDOW_WIDTH = 504;
        private const int GRAPHICWINDOW_HEIGHT = 180;

        private const int SPECTRUMSIZE = 512;
        private const int WAVEDATASIZE = 256;

        private FMOD.System     system  = null;
        private FMOD.Sound      sound   = null;
        private FMOD.Channel    channel = null;

        private float[] spectrum = new float[SPECTRUMSIZE];
        private float[] wavedata = new float[WAVEDATASIZE];

        private SolidBrush      mBrushBlack, mBrushWhite, mBrushGreen;

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.TextBox textBox;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button pauseButton;
        private System.Windows.Forms.Button playButton;
        private System.Windows.Forms.Button exit_button;
        private System.Windows.Forms.Timer timer;
        private System.ComponentModel.IContainer components;

		public Form1()
		{
			InitializeComponent();

            this.SetStyle(ControlStyles.AllPaintingInWmPaint | ControlStyles.UserPaint | ControlStyles.DoubleBuffer,true);
		}

		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
                FMOD.RESULT     result;
            
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
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.textBox = new System.Windows.Forms.TextBox();
            this.button1 = new System.Windows.Forms.Button();
            this.pauseButton = new System.Windows.Forms.Button();
            this.playButton = new System.Windows.Forms.Button();
            this.exit_button = new System.Windows.Forms.Button();
            this.timer = new System.Windows.Forms.Timer(this.components);
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.button1);
            this.groupBox1.Controls.Add(this.textBox);
            this.groupBox1.Location = new System.Drawing.Point(16, 184);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(464, 88);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Audio file to play";
            // 
            // textBox
            // 
            this.textBox.Location = new System.Drawing.Point(8, 26);
            this.textBox.Name = "textBox";
            this.textBox.ReadOnly = true;
            this.textBox.Size = new System.Drawing.Size(408, 20);
            this.textBox.TabIndex = 0;
            this.textBox.Text = "";
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(424, 24);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(32, 24);
            this.button1.TabIndex = 1;
            this.button1.Text = "...";
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // pauseButton
            // 
            this.pauseButton.Location = new System.Drawing.Point(252, 240);
            this.pauseButton.Name = "pauseButton";
            this.pauseButton.Size = new System.Drawing.Size(124, 24);
            this.pauseButton.TabIndex = 28;
            this.pauseButton.Text = "Pause";
            this.pauseButton.Click += new System.EventHandler(this.pauseButton_Click);
            // 
            // playButton
            // 
            this.playButton.Location = new System.Drawing.Point(112, 240);
            this.playButton.Name = "playButton";
            this.playButton.Size = new System.Drawing.Size(128, 24);
            this.playButton.TabIndex = 27;
            this.playButton.Text = "Play";
            this.playButton.Click += new System.EventHandler(this.playButton_Click);
            // 
            // exit_button
            // 
            this.exit_button.Location = new System.Drawing.Point(208, 280);
            this.exit_button.Name = "exit_button";
            this.exit_button.Size = new System.Drawing.Size(72, 24);
            this.exit_button.TabIndex = 26;
            this.exit_button.Text = "Exit";
            this.exit_button.Click += new System.EventHandler(this.exit_button_Click);
            // 
            // timer
            // 
            this.timer.Enabled = true;
            this.timer.Interval = 10;
            this.timer.Tick += new System.EventHandler(this.timer_Tick);
            // 
            // Form1
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(496, 315);
            this.Controls.Add(this.pauseButton);
            this.Controls.Add(this.playButton);
            this.Controls.Add(this.exit_button);
            this.Controls.Add(this.groupBox1);
            this.Name = "Form1";
            this.Text = "Spectrum Example";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.groupBox1.ResumeLayout(false);
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

            mBrushBlack = new SolidBrush(Color.Black);
            mBrushGreen = new SolidBrush(Color.Green);
            mBrushWhite = new SolidBrush(Color.White);

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

        private void button1_Click(object sender, System.EventArgs e)
        {
            OpenFileDialog dialog = new OpenFileDialog();

            if (sound != null)
            {
                if (channel != null)
                {
                    channel.stop();
                    channel = null;
                }
                sound.release();
                sound = null;
            }

            if(dialog.ShowDialog() == DialogResult.OK)
            {
                FMOD.RESULT result;

                result = system.createStream(dialog.FileName, FMOD.MODE.SOFTWARE | FMOD.MODE._2D, ref sound);
                ERRCHECK(result);

                textBox.Text = dialog.FileName;
            }

            playButton.Text = "Play";
        }

        private void playButton_Click(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;
            bool        isplaying = false;
            
            if (channel != null)
            {
                channel.isPlaying(ref isplaying);
            }

            if (sound != null && !isplaying)
            {
                result = system.playSound(FMOD.CHANNELINDEX.FREE, sound, false, ref channel);
                ERRCHECK(result);

                playButton.Text = "Stop";
            }
            else
            {
                if (channel != null)
                {
                    channel.stop();
                    channel = null;
                }

                playButton.Text = "Play";
            }
        }

        private void pauseButton_Click(object sender, System.EventArgs e)
        {
            if (channel != null)
            {
                bool ispaused = false;

                channel.getPaused(ref ispaused);
                channel.setPaused(!ispaused);

                if (ispaused)
                {
                    pauseButton.Text = "Pause";
                }
                else
                {
                    pauseButton.Text = "Resume";
                }
            }
        }

        private void timer_Tick(object sender, System.EventArgs e)
        {
            Invalidate();

            if (system != null)
            {
                system.update();
            }

            if (channel != null)
            {
                bool isplaying = false;

                channel.isPlaying(ref isplaying);

                if (!isplaying)
                {
                    playButton.Text = "Play";
                }
            }
        }

        private void drawSpectrum(Graphics g)
        {
            int numchannels                   = 0;
            int dummy                         = 0;
            FMOD.SOUND_FORMAT dummyformat     = FMOD.SOUND_FORMAT.NONE;
            FMOD.DSP_RESAMPLER dummyresampler = FMOD.DSP_RESAMPLER.LINEAR;
            int count                         = 0;
            int count2                        = 0;


            system.getSoftwareFormat(ref dummy, ref dummyformat, ref numchannels, ref dummy ,ref dummyresampler, ref dummy);

            /*
                    DRAW SPECTRUM
            */
            for (count = 0; count < numchannels; count++)
            {
                float max = 0;

                system.getSpectrum(spectrum, SPECTRUMSIZE, count, FMOD.DSP_FFT_WINDOW.TRIANGLE);
                    
                for (count2 = 0; count2 < 255; count2++)
                {
                    if (max < spectrum[count2])
                    {
                        max = spectrum[count2];
                    }
                }
                    
                /*
                    The upper band of frequencies at 44khz is pretty boring (ie 11-22khz), so we are only
                    going to display the first 256 frequencies, or (0-11khz) 
                */
                for (count2 = 0; count2 < 255; count2++)
                {
                    float height;

                    height = spectrum[count2] / max * GRAPHICWINDOW_HEIGHT;

                    if (height >= GRAPHICWINDOW_HEIGHT)
                    {
                        height = GRAPHICWINDOW_HEIGHT - 1;
                    }

                    if (height < 0)
                    {
                        height = 0;
                    }

                    height = GRAPHICWINDOW_HEIGHT - height;

                    g.FillRectangle(mBrushGreen, count2, height, 1.0f, GRAPHICWINDOW_HEIGHT - height);
                }
            }
        }

        private void drawOscilliscope(Graphics g)
        {
            int numchannels                   = 0;
            int dummy                         = 0;
            FMOD.SOUND_FORMAT dummyformat     = FMOD.SOUND_FORMAT.NONE;
            FMOD.DSP_RESAMPLER dummyresampler = FMOD.DSP_RESAMPLER.LINEAR;
            int count                         = 0;
            int count2                        = 0;

            system.getSoftwareFormat(ref dummy, ref dummyformat, ref numchannels, ref dummy ,ref dummyresampler, ref dummy);

            /*
                    DRAW WAVEDATA
            */
            for (count = 0; count < numchannels; count++)
            {
                system.getWaveData(wavedata, WAVEDATASIZE, count);

                for (count2 = 0; count2 < WAVEDATASIZE-1; count2++)
                {
                    float y;

                    y = (wavedata[count2] + 1) / 2.0f * GRAPHICWINDOW_HEIGHT;

                    g.FillRectangle(mBrushWhite, count2 + GRAPHICWINDOW_WIDTH - WAVEDATASIZE - 10.0f, y, 1.0f, 1.0f); 
                }
            }
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            e.Graphics.FillRectangle(mBrushBlack, 0, 0, GRAPHICWINDOW_WIDTH, GRAPHICWINDOW_HEIGHT);

            if (system != null)
            {
                drawSpectrum(e.Graphics);
                drawOscilliscope(e.Graphics);
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
