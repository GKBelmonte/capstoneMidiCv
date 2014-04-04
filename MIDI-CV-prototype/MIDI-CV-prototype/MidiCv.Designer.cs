namespace MIDI_CV_prototype
{
    partial class MidiCv
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MidiCv));
            this.serialPort = new System.IO.Ports.SerialPort(this.components);
            this.cbSerialSelect = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.button1 = new System.Windows.Forms.Button();
            this.bSerialBegin = new System.Windows.Forms.Button();
            this.tbSendByte = new System.Windows.Forms.TextBox();
            this.l_send = new System.Windows.Forms.Label();
            this.bSend = new System.Windows.Forms.Button();
            this.tbReceived = new System.Windows.Forms.TextBox();
            this.pbKeyboard = new System.Windows.Forms.PictureBox();
            this.button_up = new System.Windows.Forms.Button();
            this.button_right = new System.Windows.Forms.Button();
            this.button_left = new System.Windows.Forms.Button();
            this.button_bot = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.pbKeyboard)).BeginInit();
            this.SuspendLayout();
            // 
            // serialPort
            // 
            this.serialPort.BaudRate = 31250;
            // 
            // cbSerialSelect
            // 
            this.cbSerialSelect.FormattingEnabled = true;
            this.cbSerialSelect.Location = new System.Drawing.Point(95, 17);
            this.cbSerialSelect.Name = "cbSerialSelect";
            this.cbSerialSelect.Size = new System.Drawing.Size(121, 21);
            this.cbSerialSelect.TabIndex = 0;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(34, 20);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(58, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Serial Port:";
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(236, 16);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(86, 21);
            this.button1.TabIndex = 2;
            this.button1.Text = "Sniff...";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.sniff_click);
            // 
            // bSerialBegin
            // 
            this.bSerialBegin.Location = new System.Drawing.Point(342, 16);
            this.bSerialBegin.Name = "bSerialBegin";
            this.bSerialBegin.Size = new System.Drawing.Size(86, 21);
            this.bSerialBegin.TabIndex = 3;
            this.bSerialBegin.Text = "Begin";
            this.bSerialBegin.UseVisualStyleBackColor = true;
            this.bSerialBegin.Click += new System.EventHandler(this.bSerialBegin_Click);
            // 
            // tbSendByte
            // 
            this.tbSendByte.Location = new System.Drawing.Point(95, 44);
            this.tbSendByte.Name = "tbSendByte";
            this.tbSendByte.Size = new System.Drawing.Size(121, 20);
            this.tbSendByte.TabIndex = 4;
            // 
            // l_send
            // 
            this.l_send.AutoSize = true;
            this.l_send.Location = new System.Drawing.Point(34, 47);
            this.l_send.Name = "l_send";
            this.l_send.Size = new System.Drawing.Size(59, 13);
            this.l_send.TabIndex = 5;
            this.l_send.Text = "Send Byte:";
            // 
            // bSend
            // 
            this.bSend.Location = new System.Drawing.Point(236, 43);
            this.bSend.Name = "bSend";
            this.bSend.Size = new System.Drawing.Size(86, 21);
            this.bSend.TabIndex = 6;
            this.bSend.Text = "Send";
            this.bSend.UseVisualStyleBackColor = true;
            this.bSend.Click += new System.EventHandler(this.send_click);
            // 
            // tbReceived
            // 
            this.tbReceived.Location = new System.Drawing.Point(37, 70);
            this.tbReceived.Multiline = true;
            this.tbReceived.Name = "tbReceived";
            this.tbReceived.ReadOnly = true;
            this.tbReceived.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.tbReceived.Size = new System.Drawing.Size(391, 93);
            this.tbReceived.TabIndex = 7;
            // 
            // pbKeyboard
            // 
            this.pbKeyboard.Image = ((System.Drawing.Image)(resources.GetObject("pbKeyboard.Image")));
            this.pbKeyboard.ImageLocation = "";
            this.pbKeyboard.InitialImage = ((System.Drawing.Image)(resources.GetObject("pbKeyboard.InitialImage")));
            this.pbKeyboard.Location = new System.Drawing.Point(37, 169);
            this.pbKeyboard.Name = "pbKeyboard";
            this.pbKeyboard.Size = new System.Drawing.Size(251, 190);
            this.pbKeyboard.TabIndex = 8;
            this.pbKeyboard.TabStop = false;
            // 
            // button_up
            // 
            this.button_up.Location = new System.Drawing.Point(385, 197);
            this.button_up.Name = "button_up";
            this.button_up.Size = new System.Drawing.Size(25, 25);
            this.button_up.TabIndex = 9;
            this.button_up.UseVisualStyleBackColor = true;
            this.button_up.Click += new System.EventHandler(this.button_up_Click);
            // 
            // button_right
            // 
            this.button_right.Location = new System.Drawing.Point(416, 222);
            this.button_right.Name = "button_right";
            this.button_right.Size = new System.Drawing.Size(25, 25);
            this.button_right.TabIndex = 10;
            this.button_right.UseVisualStyleBackColor = true;
            this.button_right.Click += new System.EventHandler(this.button_right_Click);
            // 
            // button_left
            // 
            this.button_left.Location = new System.Drawing.Point(356, 222);
            this.button_left.Name = "button_left";
            this.button_left.Size = new System.Drawing.Size(25, 25);
            this.button_left.TabIndex = 11;
            this.button_left.UseVisualStyleBackColor = true;
            this.button_left.Click += new System.EventHandler(this.button_left_Click);
            // 
            // button_bot
            // 
            this.button_bot.Location = new System.Drawing.Point(385, 248);
            this.button_bot.Name = "button_bot";
            this.button_bot.Size = new System.Drawing.Size(25, 25);
            this.button_bot.TabIndex = 12;
            this.button_bot.UseVisualStyleBackColor = true;
            this.button_bot.Click += new System.EventHandler(this.button_bot_Click);
            // 
            // MidiCv
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(515, 365);
            this.Controls.Add(this.button_bot);
            this.Controls.Add(this.button_left);
            this.Controls.Add(this.button_right);
            this.Controls.Add(this.button_up);
            this.Controls.Add(this.pbKeyboard);
            this.Controls.Add(this.tbReceived);
            this.Controls.Add(this.bSend);
            this.Controls.Add(this.l_send);
            this.Controls.Add(this.tbSendByte);
            this.Controls.Add(this.bSerialBegin);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.cbSerialSelect);
            this.Name = "MidiCv";
            this.Text = "MIDI USB";
            ((System.ComponentModel.ISupportInitialize)(this.pbKeyboard)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.IO.Ports.SerialPort serialPort;
        private System.Windows.Forms.ComboBox cbSerialSelect;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button bSerialBegin;
        private System.Windows.Forms.TextBox tbSendByte;
        private System.Windows.Forms.Label l_send;
        private System.Windows.Forms.Button bSend;
        private System.Windows.Forms.TextBox tbReceived;
        private System.Windows.Forms.PictureBox pbKeyboard;
        private System.Windows.Forms.Button button_up;
        private System.Windows.Forms.Button button_right;
        private System.Windows.Forms.Button button_left;
        private System.Windows.Forms.Button button_bot;
    }
}

