namespace animationApp {
    partial class PreferencesDialog {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing) {
            if(disposing && (components != null)) {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent() {
            buttonOk = new Button();
            checkBox1 = new CheckBox();
            SuspendLayout();
            // 
            // buttonOk
            // 
            buttonOk.Location = new Point(263, 220);
            buttonOk.Name = "buttonOk";
            buttonOk.Size = new Size(75, 23);
            buttonOk.TabIndex = 0;
            buttonOk.Text = "Ok";
            buttonOk.UseVisualStyleBackColor = true;
            buttonOk.Click += buttonOk_Click;
            // 
            // checkBox1
            // 
            checkBox1.AutoSize = true;
            checkBox1.Location = new Point(12, 12);
            checkBox1.Name = "checkBox1";
            checkBox1.Size = new Size(164, 19);
            checkBox1.TabIndex = 1;
            checkBox1.Text = "Center View / Corner View";
            checkBox1.UseVisualStyleBackColor = true;
            checkBox1.CheckedChanged += checkBox1_CheckedChanged;
            // 
            // PreferencesDialog
            // 
            AcceptButton = buttonOk;
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(349, 251);
            Controls.Add(checkBox1);
            Controls.Add(buttonOk);
            FormBorderStyle = FormBorderStyle.FixedDialog;
            MaximizeBox = false;
            MinimizeBox = false;
            Name = "PreferencesDialog";
            Text = "PreferencesDialog";
            Load += PreferencesDialog_Load;
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private Button buttonOk;
        private CheckBox checkBox1;
    }
}