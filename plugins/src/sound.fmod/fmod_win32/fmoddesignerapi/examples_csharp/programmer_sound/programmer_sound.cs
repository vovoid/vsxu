using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.Text;
using System.Runtime.InteropServices;

namespace programmer_sound
{
	public class Form1 : System.Windows.Forms.Form
	{
        private System.Windows.Forms.Button exit_button;
        private System.Windows.Forms.StatusBar statusBar;
        private System.Windows.Forms.Label label;
        private System.Windows.Forms.Button startEvent;
        private System.Windows.Forms.Timer timer1;
        private System.ComponentModel.IContainer components;

        private bool initialised    = false;
        private bool eventstart     = false;
        private bool exit           = false;

        private static FMOD.EVENT_CALLBACK eventcallback = new FMOD.EVENT_CALLBACK(FMOD_EVENT_CALLBACK);

        private static FMOD.RESULT FMOD_EVENT_CALLBACK(IntPtr eventraw, FMOD.EVENT_CALLBACKTYPE type, IntPtr param1, IntPtr param2, IntPtr userdata)
        {
            unsafe
            {           
                switch (type)
                {
                    case FMOD.EVENT_CALLBACKTYPE.SOUNDDEF_CREATE :
                    {
                        int entryindex   = *(int*)param2.ToPointer() ;     // [in]  (int) index of sound definition entry
                        uint *realpointer = (uint *)param2.ToPointer();    // [out] (FMOD::Sound *) a valid lower level API FMOD Sound handle

                        FMOD.Sound s = null;
                        fsb.getSubSound(entryindex, ref s);
           
                        *realpointer = (uint)s.getRaw().ToPointer();

                        break;
                    }

                    case FMOD.EVENT_CALLBACKTYPE.SOUNDDEF_RELEASE :
                    {
                        break;
                    }
                }

            }
            return FMOD.RESULT.OK;
        }

        /*
            ALL FMOD CALLS MUST HAPPEN WITHIN THE SAME THREAD.
            WE WILL DO EVERYTHING IN THE TIMER THREAD 
        */
        static FMOD.Sound   fsb         = null;
        FMOD.System         sys         = null;
        FMOD.EventSystem    eventsystem = null;
        FMOD.EventGroup     eventgroup  = null;
        FMOD.Event          _event      = null;

        private void timer1_Tick(object sender, System.EventArgs e)
        {
            FMOD.RESULT         result;

            if (!initialised)
            {
                result = FMOD.Event_Factory.EventSystem_Create(ref eventsystem);
                ERRCHECK(result);
                result = eventsystem.init(64, FMOD.INITFLAG.NORMAL, (IntPtr)null, FMOD.EVENT_INIT.NORMAL);
                ERRCHECK(result);
                result = eventsystem.setMediaPath("../../../../examples/media/");
                ERRCHECK(result);
                result = eventsystem.load("examples.fev");
                ERRCHECK(result);
                result = eventsystem.getGroup("examples/examples", false, ref eventgroup);
                ERRCHECK(result);

                result = eventsystem.getSystemObject(ref sys);
                ERRCHECK(result);
                result = sys.createStream("../../../../examples/media/other.fsb", (FMOD.MODE._2D | FMOD.MODE.SOFTWARE), ref fsb);
                ERRCHECK(result);

                initialised = true;
            }

            /*
                "Main Loop" 
            */
            result = eventsystem.update();
            ERRCHECK(result);

            if (eventstart)
            {
                result = eventgroup.getEvent("programmersound", FMOD.EVENT_MODE.DEFAULT, ref _event);
                if (result == FMOD.RESULT.OK)
                {
                    result = _event.setCallback(eventcallback, (IntPtr)null);
                    ERRCHECK(result);
                    result = _event.start();
                    ERRCHECK(result);
                }

                eventstart = false;
            }

            /*
                Cleanup and exit 
            */
            if (exit)
            {
                result = eventsystem.unload();
                ERRCHECK(result);
                result = fsb.release();
                ERRCHECK(result);
                result = eventsystem.release();
                ERRCHECK(result);

                Application.Exit();
            }
        }

		public Form1()
		{
			InitializeComponent();
		}


		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
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
            this.exit_button = new System.Windows.Forms.Button();
            this.statusBar = new System.Windows.Forms.StatusBar();
            this.label = new System.Windows.Forms.Label();
            this.startEvent = new System.Windows.Forms.Button();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.SuspendLayout();
            // 
            // exit_button
            // 
            this.exit_button.Location = new System.Drawing.Point(168, 48);
            this.exit_button.Name = "exit_button";
            this.exit_button.Size = new System.Drawing.Size(72, 24);
            this.exit_button.TabIndex = 19;
            this.exit_button.Text = "Exit";
            this.exit_button.Click += new System.EventHandler(this.exit_button_Click);
            // 
            // statusBar
            // 
            this.statusBar.Location = new System.Drawing.Point(0, 86);
            this.statusBar.Name = "statusBar";
            this.statusBar.Size = new System.Drawing.Size(292, 24);
            this.statusBar.TabIndex = 20;
            // 
            // label
            // 
            this.label.Location = new System.Drawing.Point(16, 8);
            this.label.Name = "label";
            this.label.Size = new System.Drawing.Size(264, 32);
            this.label.TabIndex = 21;
            this.label.Text = "Copyright (c) Firelight Technologies 2004-2007";
            this.label.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // startEvent
            // 
            this.startEvent.Location = new System.Drawing.Point(48, 48);
            this.startEvent.Name = "startEvent";
            this.startEvent.Size = new System.Drawing.Size(72, 24);
            this.startEvent.TabIndex = 22;
            this.startEvent.Text = "Start Event";
            this.startEvent.Click += new System.EventHandler(this.startEvent_Click);
            // 
            // timer1
            // 
            this.timer1.Enabled = true;
            this.timer1.Interval = 10;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // Form1
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(292, 110);
            this.Controls.Add(this.startEvent);
            this.Controls.Add(this.label);
            this.Controls.Add(this.statusBar);
            this.Controls.Add(this.exit_button);
            this.Name = "Form1";
            this.Text = "Programmer Sound Example";
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

        private void startEvent_Click(object sender, System.EventArgs e)
        {
            eventstart = true;
        }

        private void exit_button_Click(object sender, System.EventArgs e)
        {
            exit = true;
        }
	}
}
