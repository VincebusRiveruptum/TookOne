namespace animationApp {
    partial class NewFileDialog {
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
            colorDepthCombo = new ComboBox();
            widthNumeric = new NumericUpDown();
            heightNumeric = new NumericUpDown();
            label1 = new Label();
            label2 = new Label();
            label3 = new Label();
            createButton = new Button();
            cancelButton = new Button();
            ((System.ComponentModel.ISupportInitialize)widthNumeric).BeginInit();
            ((System.ComponentModel.ISupportInitialize)heightNumeric).BeginInit();
            SuspendLayout();
            // 
            // colorDepthCombo
            // 
            colorDepthCombo.DropDownStyle = ComboBoxStyle.DropDownList;
            colorDepthCombo.FormattingEnabled = true;
            colorDepthCombo.Location = new Point(103, 84);
            colorDepthCombo.Name = "colorDepthCombo";
            colorDepthCombo.Size = new Size(151, 23);
            colorDepthCombo.TabIndex = 0;
            // 
            // widthNumeric
            // 
            widthNumeric.Location = new Point(134, 26);
            widthNumeric.Maximum = new decimal(new int[] { 10000, 0, 0, 0 });
            widthNumeric.Name = "widthNumeric";
            widthNumeric.Size = new Size(120, 23);
            widthNumeric.TabIndex = 1;
            widthNumeric.ValueChanged += numericUpDown1_ValueChanged_1;
            // 
            // heightNumeric
            // 
            heightNumeric.Location = new Point(134, 55);
            heightNumeric.Maximum = new decimal(new int[] { 10000, 0, 0, 0 });
            heightNumeric.Name = "heightNumeric";
            heightNumeric.Size = new Size(120, 23);
            heightNumeric.TabIndex = 2;
            // 
            // label1
            // 
            label1.AutoSize = true;
            label1.Location = new Point(103, 28);
            label1.Name = "label1";
            label1.Size = new Size(20, 15);
            label1.TabIndex = 3;
            label1.Text = "X :";
            // 
            // label2
            // 
            label2.AutoSize = true;
            label2.Location = new Point(103, 57);
            label2.Name = "label2";
            label2.Size = new Size(20, 15);
            label2.TabIndex = 4;
            label2.Text = "Y :";
            // 
            // label3
            // 
            label3.AutoSize = true;
            label3.Location = new Point(21, 87);
            label3.Name = "label3";
            label3.Size = new Size(76, 15);
            label3.TabIndex = 5;
            label3.Text = "Color depth :";
            // 
            // createButton
            // 
            createButton.Location = new Point(179, 143);
            createButton.Name = "createButton";
            createButton.Size = new Size(75, 23);
            createButton.TabIndex = 6;
            createButton.Text = "Create";
            createButton.UseVisualStyleBackColor = true;
            createButton.Click += createButton_Click;
            // 
            // cancelButton
            // 
            cancelButton.Location = new Point(12, 143);
            cancelButton.Name = "cancelButton";
            cancelButton.Size = new Size(75, 23);
            cancelButton.TabIndex = 7;
            cancelButton.Text = "Cancel";
            cancelButton.UseVisualStyleBackColor = true;
            cancelButton.Click += cancelButton_Click;
            // 
            // NewFileDialog
            // 
            AcceptButton = createButton;
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            CancelButton = cancelButton;
            ClientSize = new Size(266, 178);
            Controls.Add(cancelButton);
            Controls.Add(createButton);
            Controls.Add(label3);
            Controls.Add(label2);
            Controls.Add(label1);
            Controls.Add(heightNumeric);
            Controls.Add(widthNumeric);
            Controls.Add(colorDepthCombo);
            FormBorderStyle = FormBorderStyle.FixedDialog;
            MaximizeBox = false;
            MinimizeBox = false;
            Name = "NewFileDialog";
            ShowInTaskbar = false;
            StartPosition = FormStartPosition.CenterParent;
            Text = "New file...";
            Load += NewFileDialog_Load;
            ((System.ComponentModel.ISupportInitialize)widthNumeric).EndInit();
            ((System.ComponentModel.ISupportInitialize)heightNumeric).EndInit();
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private ComboBox colorDepthCombo;
        private NumericUpDown widthNumeric;
        private NumericUpDown heightNumeric;
        private Label label1;
        private Label label2;
        private Label label3;
        private Button createButton;
        private Button cancelButton;
    }
}