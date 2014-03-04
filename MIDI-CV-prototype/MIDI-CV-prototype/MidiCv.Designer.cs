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
            this.serialPort = new System.IO.Ports.SerialPort(this.components);
            this.cbSerialSelect = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.button1 = new System.Windows.Forms.Button();
            this.bSerialBegin = new System.Windows.Forms.Button();
            this.tbSendByte = new System.Windows.Forms.TextBox();
            this.l_send = new System.Windows.Forms.Label();
            this.bSend = new System.Windows.Forms.Button();
            this.tbReceived = new System.Windows.Forms.TextBox();
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
            this.tbReceived.Size = new System.Drawing.Size(391, 159);
            this.tbReceived.TabIndex = 7;
            // 
            // MidiCv
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(515, 241);
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
    }
}

