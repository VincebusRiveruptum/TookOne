namespace animationApp {
    partial class FillPatternDialog {
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
            pictureBox1 = new PictureBox();
            buttonOk = new Button();
            buttonCancel = new Button();
            buttonInvertColor = new Button();
            groupBox1 = new GroupBox();
            buttonRemove = new Button();
            buttonAdd = new Button();
            listBox1 = new ListBox();
            ((System.ComponentModel.ISupportInitialize)pictureBox1).BeginInit();
            groupBox1.SuspendLayout();
            SuspendLayout();
            // 
            // pictureBox1
            // 
            pictureBox1.BorderStyle = BorderStyle.Fixed3D;
            pictureBox1.Location = new Point(15, 22);
            pictureBox1.Name = "pictureBox1";
            pictureBox1.Size = new Size(256, 256);
            pictureBox1.TabIndex = 0;
            pictureBox1.TabStop = false;
            // 
            // buttonOk
            // 
            buttonOk.Location = new Point(452, 309);
            buttonOk.Name = "buttonOk";
            buttonOk.Size = new Size(75, 23);
            buttonOk.TabIndex = 1;
            buttonOk.Text = "Ok";
            buttonOk.UseVisualStyleBackColor = true;
            // 
            // buttonCancel
            // 
            buttonCancel.Location = new Point(371, 309);
            buttonCancel.Name = "buttonCancel";
            buttonCancel.Size = new Size(75, 23);
            buttonCancel.TabIndex = 2;
            buttonCancel.Text = "Cancel";
            buttonCancel.UseVisualStyleBackColor = true;
            // 
            // buttonInvertColor
            // 
            buttonInvertColor.Location = new Point(418, 241);
            buttonInvertColor.Name = "buttonInvertColor";
            buttonInvertColor.Size = new Size(91, 23);
            buttonInvertColor.TabIndex = 3;
            buttonInvertColor.Text = "Invert Color";
            buttonInvertColor.UseVisualStyleBackColor = true;
            // 
            // groupBox1
            // 
            groupBox1.Controls.Add(buttonRemove);
            groupBox1.Controls.Add(buttonAdd);
            groupBox1.Controls.Add(listBox1);
            groupBox1.Controls.Add(buttonInvertColor);
            groupBox1.Controls.Add(pictureBox1);
            groupBox1.Location = new Point(12, 12);
            groupBox1.Name = "groupBox1";
            groupBox1.Size = new Size(515, 291);
            groupBox1.TabIndex = 4;
            groupBox1.TabStop = false;
            groupBox1.Text = "Preview";
            // 
            // buttonRemove
            // 
            buttonRemove.Location = new Point(418, 212);
            buttonRemove.Name = "buttonRemove";
            buttonRemove.Size = new Size(91, 23);
            buttonRemove.TabIndex = 6;
            buttonRemove.Text = "Remove";
            buttonRemove.UseVisualStyleBackColor = true;
            // 
            // buttonAdd
            // 
            buttonAdd.Location = new Point(321, 212);
            buttonAdd.Name = "buttonAdd";
            buttonAdd.Size = new Size(91, 23);
            buttonAdd.TabIndex = 5;
            buttonAdd.Text = "Add";
            buttonAdd.UseVisualStyleBackColor = true;
            // 
            // listBox1
            // 
            listBox1.FormattingEnabled = true;
            listBox1.ItemHeight = 15;
            listBox1.Location = new Point(277, 22);
            listBox1.Name = "listBox1";
            listBox1.Size = new Size(232, 184);
            listBox1.TabIndex = 4;
            // 
            // FillPatternDialog
            // 
            AcceptButton = buttonOk;
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            CancelButton = buttonCancel;
            ClientSize = new Size(539, 343);
            Controls.Add(groupBox1);
            Controls.Add(buttonCancel);
            Controls.Add(buttonOk);
            FormBorderStyle = FormBorderStyle.FixedDialog;
            MaximizeBox = false;
            MinimizeBox = false;
            Name = "FillPatternDialog";
            Text = "Edit fill patterns";
            ((System.ComponentModel.ISupportInitialize)pictureBox1).EndInit();
            groupBox1.ResumeLayout(false);
            ResumeLayout(false);
        }

        #endregion

        private PictureBox pictureBox1;
        private Button buttonOk;
        private Button buttonCancel;
        private Button buttonInvertColor;
        private GroupBox groupBox1;
        private Button buttonRemove;
        private Button buttonAdd;
        private ListBox listBox1;
    }
}