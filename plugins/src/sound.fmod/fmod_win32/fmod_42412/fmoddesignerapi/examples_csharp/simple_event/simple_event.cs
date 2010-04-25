/*=======================================================================
  Simple Event Example.  Copyright (c) Firelight Technologies 2004-2007.

 This example plays an event created with the FMOD Designer sound
 designer tool.  It simply plays an event, retrieves the parameters
 and allows the user to adjust them.
=========================================================================*/

using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

namespace simple_event
{
	public class Form1 : System.Windows.Forms.Form
	{
        private System.Windows.Forms.TrackBar trackBarPosition;
        private System.Windows.Forms.Label label;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.Button exit_button;
        private System.Windows.Forms.StatusBar statusBar;
        private System.ComponentModel.IContainer components;

        private bool initialised    = false;
        private bool trackbarscroll = false;
        private bool exit           = false;

        /*
            ALL FMOD CALLS MUST HAPPEN WITHIN THE SAME THREAD.
            WE WILL DO EVERYTHING IN THE TIMER THREAD 
        */
        FMOD.EventSystem    eventsystem    = null;
        FMOD.EventGroup     eventgroup     = null;
        FMOD.EventCategory  mastercategory = null;
        FMOD.Event          car            = null;
        FMOD.EventParameter rpm            = null;
        FMOD.EventParameter load           = null;
        FMOD.RESULT         result;
        float               rangemin, rangemax;
        private System.Windows.Forms.Label label1;

        private void timer1_Tick(object sender, System.EventArgs e)
        {
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

                result = eventsystem.getGroup("examples/examples/car", false, ref eventgroup);
                ERRCHECK(result);
                result = eventgroup.getEvent("car", FMOD.EVENT_MODE.DEFAULT, ref car);
                ERRCHECK(result);

                result = eventsystem.getCategory("master", ref mastercategory);
                ERRCHECK(result);

                result = car.getParameter("load", ref load);
                ERRCHECK(result);
                result = load.getRange(ref rangemin, ref rangemax);
                ERRCHECK(result);
                result = load.setValue(rangemax);
                ERRCHECK(result);

                result = car.getParameterByIndex(0, ref rpm);
                ERRCHECK(result);
                result = rpm.getRange(ref rangemin, ref rangemax);
                ERRCHECK(result);
                result = rpm.setValue(1000.0f);
                ERRCHECK(result);

                trackBarPosition.Minimum = (int)rangemin;
                trackBarPosition.Maximum = (int)rangemax;

                trackBarPosition.Value = 1000;

                result = car.start();
                ERRCHECK(result);

                initialised = true;
            }


            /*
                "Main Loop" 
            */
            if (trackbarscroll)
            {
                result = rpm.setValue((float)trackBarPosition.Value);
                ERRCHECK(result);

                trackbarscroll = false;
            }
        
            float rpmvalue = 0.0f;

            result = rpm.getValue(ref rpmvalue);
            ERRCHECK(result);

            statusBar.Text = "RPM Value = " + rpmvalue;

            result = eventsystem.update();
            ERRCHECK(result);

            /*
                Clean up and exit 
            */
            if (exit)
            {
                result = eventgroup.freeEventData();
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
            this.trackBarPosition = new System.Windows.Forms.TrackBar();
            this.label = new System.Windows.Forms.Label();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.exit_button = new System.Windows.Forms.Button();
            this.statusBar = new System.Windows.Forms.StatusBar();
            this.label1 = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.trackBarPosition)).BeginInit();
            this.SuspendLayout();
            // 
            // trackBarPosition
            // 
            this.trackBarPosition.Location = new System.Drawing.Point(64, 64);
            this.trackBarPosition.Maximum = 1000;
            this.trackBarPosition.Name = "trackBarPosition";
            this.trackBarPosition.Size = new System.Drawing.Size(216, 45);
            this.trackBarPosition.TabIndex = 1;
            this.trackBarPosition.TickStyle = System.Windows.Forms.TickStyle.None;
            this.trackBarPosition.Scroll += new System.EventHandler(this.trackBarPosition_Scroll);
            // 
            // label
            // 
            this.label.Location = new System.Drawing.Point(16, 16);
            this.label.Name = "label";
            this.label.Size = new System.Drawing.Size(264, 32);
            this.label.TabIndex = 17;
            this.label.Text = "Copyright (c) Firelight Technologies 2004-2007";
            this.label.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // timer1
            // 
            this.timer1.Enabled = true;
            this.timer1.Interval = 10;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // exit_button
            // 
            this.exit_button.Location = new System.Drawing.Point(112, 120);
            this.exit_button.Name = "exit_button";
            this.exit_button.Size = new System.Drawing.Size(72, 24);
            this.exit_button.TabIndex = 18;
            this.exit_button.Text = "Exit";
            this.exit_button.Click += new System.EventHandler(this.exit_button_Click);
            // 
            // statusBar
            // 
            this.statusBar.Location = new System.Drawing.Point(0, 158);
            this.statusBar.Name = "statusBar";
            this.statusBar.Size = new System.Drawing.Size(292, 24);
            this.statusBar.TabIndex = 19;
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(16, 64);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(34, 32);
            this.label1.TabIndex = 20;
            this.label1.Text = "RPM:";
            this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Form1
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(292, 182);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.statusBar);
            this.Controls.Add(this.exit_button);
            this.Controls.Add(this.label);
            this.Controls.Add(this.trackBarPosition);
            this.Name = "Form1";
            this.Text = "Simple Event Example";
            this.Load += new System.EventHandler(this.Form1_Load);
            ((System.ComponentModel.ISupportInitialize)(this.trackBarPosition)).EndInit();
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

        private void trackBarPosition_Scroll(object sender, System.EventArgs e)
        {
            trackbarscroll = true;
        }

        private void exit_button_Click(object sender, System.EventArgs e)
        {
            exit = true;
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
