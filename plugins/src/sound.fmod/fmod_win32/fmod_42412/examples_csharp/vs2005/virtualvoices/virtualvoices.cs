/*===============================================================================================
 Virtual Voices Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2008.

 This example shows the virtual channels of FMOD. The listener and sounce sources can be moved
 around by clicking and dragging. Sound sources will change colour depending on whether they
 are virtual or not. Red means they are real, blue means they are virtual channels.
===============================================================================================*/

using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

namespace virtualvoices
{
	public class VirtualVoices : System.Windows.Forms.Form
	{
        private const int NUMCHANNELS       = 50;
        private const int NUMREALCHANNELS   = 10;
        private const int WIDTH             = 640;
        private const int HEIGHT            = 480;

        private bool            mSelected  = false;
        private SoundSource     mSelectedSource;
        private SoundSource[]   mSoundSource;
        private Listener        mListener;

        private FMOD.System     system = null;
        private FMOD.Sound      sound = null;

        private System.Timers.Timer timer;
		private System.ComponentModel.Container components = null;

		public VirtualVoices()
		{
			InitializeComponent();

            this.SetClientSizeCore(WIDTH, HEIGHT);
            this.SetStyle(ControlStyles.AllPaintingInWmPaint | ControlStyles.UserPaint | ControlStyles.DoubleBuffer,true);
		}

		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
                FMOD.RESULT result;

                /*
                    Shutdown
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
            this.timer = new System.Timers.Timer();
            ((System.ComponentModel.ISupportInitialize)(this.timer)).BeginInit();
            // 
            // timer
            // 
            this.timer.Enabled = true;
            this.timer.Interval = 10;
            this.timer.SynchronizingObject = this;
            this.timer.Elapsed += new System.Timers.ElapsedEventHandler(this.timer_Elapsed);
            // 
            // VirtualVoices
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.BackColor = System.Drawing.Color.White;
            this.ClientSize = new System.Drawing.Size(632, 451);
            this.Name = "VirtualVoices";
            this.Text = "Virtual Voices Example. Copyright (c), Firelight Technologies Pty, Ltd 2004-2008." +
                " ";
            this.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Form_MouseDown);
            this.Load += new System.EventHandler(this.VirtualVoices_Load);
            this.MouseUp += new System.Windows.Forms.MouseEventHandler(this.Form_MouseUp);
            this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Form_MouseMove);
            ((System.ComponentModel.ISupportInitialize)(this.timer)).EndInit();

        }
		#endregion

		[STAThread]
		static void Main() 
		{
			Application.Run(new VirtualVoices());
		}

        private void VirtualVoices_Load(object sender, System.EventArgs e)
        {
            FMOD.RESULT result;

            /*
                Create a System object and initialize.
            */
            result = FMOD.Factory.System_Create(ref system);
            ERRCHECK(result);

            result = system.setSoftwareChannels(NUMREALCHANNELS);
            ERRCHECK(result);

            result = system.init(NUMCHANNELS, FMOD.INITFLAG.NORMAL, (IntPtr)null);
            ERRCHECK(result);

            result = system.createSound("../../../../../examples/media/drumloop.wav", (FMOD.MODE.SOFTWARE | FMOD.MODE._3D | FMOD.MODE.LOOP_NORMAL), ref sound);
            ERRCHECK(result);
            
            result = sound.set3DMinMaxDistance(4.0f, 10000.0f);
            ERRCHECK(result);

            /*
                Create a listener in the middle of the window 
            */
            mListener = new Listener(system, WIDTH/2, HEIGHT/2);

            /*
                Initialise all of the sound sources
            */
            Random r = new Random();

            mSoundSource = new SoundSource[NUMCHANNELS];

            for (int count = 0; count < NUMCHANNELS; count++)
            {
                mSoundSource[count] = new SoundSource(system, sound, r.Next(this.Width), r.Next(this.Height));
            }
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            FMOD.RESULT result;
            int         channels = 0;

            /*
                Draw the sound sources 
            */
            for (int count = 0; count < NUMCHANNELS; count++)
            {
                mSoundSource[count].Draw(e.Graphics);
            }

            /*
                Draw the listener 
            */
            mListener.Draw(e.Graphics);

            /*
                Print some information 
            */
            Font    font = new Font("Arial", 12);
            Brush   brush = new SolidBrush(Color.Black);

            result = system.getChannelsPlaying(ref channels);
            ERRCHECK(result);

            e.Graphics.DrawString("Channels Playing: " + channels, font, brush, 0, 0);
            e.Graphics.DrawString("Real Channels:    " + NUMREALCHANNELS + " (RED)", font, brush, 0, 15);
            e.Graphics.DrawString("Virtual Channels: " + (NUMCHANNELS - NUMREALCHANNELS) + " (BLUE)", font, brush, 0, 30);
        }
        
        private void Form_MouseDown(object  sender, System.Windows.Forms.MouseEventArgs e)
        {
            mSelectedSource = null;

            /*
                Check if the listener was selected
            */
            if (mListener.IsSelected(e.X, e.Y))
            {
                mSelected = true;
                return;
            }

            /*
                Check if a sound source was selected 
            */
            for (int count = 0; count < NUMCHANNELS; count++)
            {
                if (mSoundSource[count].IsSelected(e.X, e.Y))
                {
                    mSelectedSource = mSoundSource[count];
                    mSelected = true;
                    return;
                }
            }
        }

        private void Form_MouseUp(object  sender, System.Windows.Forms.MouseEventArgs e)
        {
            mSelected = false;
        }

        private void Form_MouseMove(object  sender, System.Windows.Forms.MouseEventArgs e)
        {
            if (mSelected)
            {
                if (mSelectedSource != null)
                {
                    mSelectedSource.SetPosition(e.X, e.Y);    
                }
                else
                {
                    mListener.SetPosition(e.X, e.Y);
                }
            }
        }

        private void timer_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
        {
            FMOD.RESULT result;

            Invalidate();

            if (system != null)
            {
                result = system.update();
                ERRCHECK(result);
            }
        }
        
        public static void ERRCHECK(FMOD.RESULT result)
        {
            if (result != FMOD.RESULT.OK)
            {
                MessageBox.Show("FMOD error! " + result + " - " + FMOD.Error.String(result));
                Environment.Exit(-1);
            }
        }
	}

    public class SoundSource
    {
        private const int SIZE            = 5;
        private const int SELECTTHRESHOLD = 5;

        private FMOD.System     mSystem = null;
        private FMOD.Channel    mChannel = null;
        private FMOD.VECTOR     mPos, mVel;
        private SolidBrush      mBrushBlue, mBrushRed;
        
        public SoundSource(FMOD.System system, FMOD.Sound sound, int posx, int posy)
        {
            FMOD.RESULT result;

            mPos.x = posx;
            mPos.y = posy;
            mPos.z = 0;

            mVel.x = 0;
            mVel.y = 0;
            mVel.z = 0;

            mSystem = system;

            result = system.playSound(FMOD.CHANNELINDEX.FREE, sound, true, ref mChannel);
            VirtualVoices.ERRCHECK(result);

            SetPosition(mPos.x, mPos.y);

            Random r = new Random(posx);

            result = mChannel.setFrequency(r.Next(22050, 44100));
            VirtualVoices.ERRCHECK(result);

            result = mChannel.setPaused(false);
            VirtualVoices.ERRCHECK(result);

            mBrushBlue = new SolidBrush(Color.Blue);
            mBrushRed  = new SolidBrush(Color.Red);
        }

        public void Draw(Graphics g)
        {
            FMOD.RESULT result;

            if (mChannel != null)
            {
                bool isvirtual = false;

                result = mChannel.isVirtual(ref isvirtual);

                if (isvirtual)
                {
                    g.FillEllipse(mBrushBlue, mPos.x - SIZE, mPos.y - SIZE, SIZE*2, SIZE*2);
                }
                else
                {
                    g.FillEllipse(mBrushRed, mPos.x - SIZE, mPos.y - SIZE, SIZE*2, SIZE*2);
                }
            }
        }

        public void SetPosition(float posx, float posy)
        {
            FMOD.RESULT result;

            mPos.x = posx;
            mPos.y = posy;
            mPos.z = 0;

            result = mChannel.set3DAttributes(ref mPos, ref mVel);
            VirtualVoices.ERRCHECK(result);
        }

        public bool IsSelected(float posx, float posy)
        {
            if (posx > mPos.x - SIZE - SELECTTHRESHOLD &&
                posx < mPos.x + SIZE + SELECTTHRESHOLD &&
                posy > mPos.y - SIZE - SELECTTHRESHOLD &&
                posy < mPos.y + SIZE + SELECTTHRESHOLD)
            {
                return true;
            }
            return false;
        }
    }

    public class Listener
    {
        private const int SIZE            = 15;
        private const int SELECTTHRESHOLD = 5;

        private FMOD.System     mSystem = null;
        private FMOD.VECTOR     mListenerPos;
        private FMOD.VECTOR     mUp, mVel, mForward;
        private SolidBrush      mBrush;

        public Listener(FMOD.System system, float posx, float posy)
        {
            mUp.x = 0;
            mUp.y = 0;
            mUp.z = 1;

            mForward.x = 1;
            mForward.y = 0;
            mForward.z = 0;

            mVel.x = 0;
            mVel.y = 0;
            mVel.z = 0;

            mSystem = system;

            SetPosition(posx, posy);

            mBrush = new SolidBrush(Color.Black);
        }

        public void Draw(Graphics g)
        {
            // Head
            g.FillEllipse(mBrush, mListenerPos.x - SIZE, mListenerPos.y - SIZE, SIZE*2, SIZE*2);

            // Nose
            g.FillEllipse(mBrush, mListenerPos.x - SIZE/2, mListenerPos.y - SIZE - 5, SIZE, SIZE);
            
            // Ears
            g.FillEllipse(mBrush, mListenerPos.x - SIZE - 5, mListenerPos.y - SIZE/2, SIZE, SIZE);
            g.FillEllipse(mBrush, mListenerPos.x + SIZE - 10, mListenerPos.y - SIZE/2, SIZE, SIZE);
        }

        public void SetPosition(float posx, float posy)
        {
            FMOD.RESULT result;

            mListenerPos.x = posx;
            mListenerPos.y = posy;
            mListenerPos.z = 0;

            result = mSystem.set3DListenerAttributes(0, ref mListenerPos, ref mVel, ref mForward, ref mUp);
            VirtualVoices.ERRCHECK(result);
        }

        public bool IsSelected(float posx, float posy)
        {
            if (posx > mListenerPos.x - SIZE - SELECTTHRESHOLD &&
                posx < mListenerPos.x + SIZE + SELECTTHRESHOLD &&
                posy > mListenerPos.y - SIZE - SELECTTHRESHOLD &&
                posy < mListenerPos.y + SIZE + SELECTTHRESHOLD)
            {
                return true;
            }
            return false;
        }
    }
}
