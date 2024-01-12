namespace animationApp {
    partial class ColorPaletteControl {
        /// <summary> 
        /// Variable del diseñador necesaria.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// Limpiar los recursos que se estén usando.
        /// </summary>
        /// <param name="disposing">true si los recursos administrados se deben desechar; false en caso contrario.</param>
        protected override void Dispose(bool disposing) {
            if(disposing && (components != null)) {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Código generado por el Diseñador de componentes

        /// <summary> 
        /// Método necesario para admitir el Diseñador. No se puede modificar
        /// el contenido de este método con el editor de código.
        /// </summary>
        private void InitializeComponent() {
            pbBgColor = new PictureBox();
            pbSecondaryColor = new PictureBox();
            pbSelectedColor = new PictureBox();
            groupBox1 = new GroupBox();
            pAvailableColors = new Panel();
            pictureBox8 = new PictureBox();
            pictureBox3 = new PictureBox();
            pictureBox7 = new PictureBox();
            pictureBox6 = new PictureBox();
            pictureBox5 = new PictureBox();
            pictureBox4 = new PictureBox();
            pictureBox2 = new PictureBox();
            pbColorN = new PictureBox();
            ((System.ComponentModel.ISupportInitialize)pbBgColor).BeginInit();
            ((System.ComponentModel.ISupportInitialize)pbSecondaryColor).BeginInit();
            ((System.ComponentModel.ISupportInitialize)pbSelectedColor).BeginInit();
            groupBox1.SuspendLayout();
            pAvailableColors.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)pictureBox8).BeginInit();
            ((System.ComponentModel.ISupportInitialize)pictureBox3).BeginInit();
            ((System.ComponentModel.ISupportInitialize)pictureBox7).BeginInit();
            ((System.ComponentModel.ISupportInitialize)pictureBox6).BeginInit();
            ((System.ComponentModel.ISupportInitialize)pictureBox5).BeginInit();
            ((System.ComponentModel.ISupportInitialize)pictureBox4).BeginInit();
            ((System.ComponentModel.ISupportInitialize)pictureBox2).BeginInit();
            ((System.ComponentModel.ISupportInitialize)pbColorN).BeginInit();
            SuspendLayout();
            // 
            // pbBgColor
            // 
            pbBgColor.BorderStyle = BorderStyle.FixedSingle;
            pbBgColor.Location = new Point(6, 22);
            pbBgColor.Name = "pbBgColor";
            pbBgColor.Size = new Size(64, 64);
            pbBgColor.TabIndex = 0;
            pbBgColor.TabStop = false;
            // 
            // pbSecondaryColor
            // 
            pbSecondaryColor.BackColor = Color.White;
            pbSecondaryColor.BorderStyle = BorderStyle.FixedSingle;
            pbSecondaryColor.Location = new Point(29, 44);
            pbSecondaryColor.Name = "pbSecondaryColor";
            pbSecondaryColor.Size = new Size(32, 32);
            pbSecondaryColor.TabIndex = 1;
            pbSecondaryColor.TabStop = false;
            // 
            // pbSelectedColor
            // 
            pbSelectedColor.BackColor = SystemColors.ActiveCaptionText;
            pbSelectedColor.BorderStyle = BorderStyle.FixedSingle;
            pbSelectedColor.Location = new Point(17, 33);
            pbSelectedColor.Name = "pbSelectedColor";
            pbSelectedColor.Size = new Size(32, 32);
            pbSelectedColor.TabIndex = 2;
            pbSelectedColor.TabStop = false;
            // 
            // groupBox1
            // 
            groupBox1.Controls.Add(pAvailableColors);
            groupBox1.Controls.Add(pbSelectedColor);
            groupBox1.Controls.Add(pbSecondaryColor);
            groupBox1.Controls.Add(pbBgColor);
            groupBox1.Location = new Point(3, 3);
            groupBox1.Name = "groupBox1";
            groupBox1.Size = new Size(144, 281);
            groupBox1.TabIndex = 0;
            groupBox1.TabStop = false;
            groupBox1.Enter += groupBox1_Enter;
            // 
            // pAvailableColors
            // 
            pAvailableColors.BorderStyle = BorderStyle.Fixed3D;
            pAvailableColors.Controls.Add(pictureBox8);
            pAvailableColors.Controls.Add(pictureBox3);
            pAvailableColors.Controls.Add(pictureBox7);
            pAvailableColors.Controls.Add(pictureBox6);
            pAvailableColors.Controls.Add(pictureBox5);
            pAvailableColors.Controls.Add(pictureBox4);
            pAvailableColors.Controls.Add(pictureBox2);
            pAvailableColors.Controls.Add(pbColorN);
            pAvailableColors.Location = new Point(6, 92);
            pAvailableColors.Name = "pAvailableColors";
            pAvailableColors.Size = new Size(132, 183);
            pAvailableColors.TabIndex = 3;
            // 
            // pictureBox8
            // 
            pictureBox8.BackColor = Color.FromArgb(255, 255, 128);
            pictureBox8.BorderStyle = BorderStyle.FixedSingle;
            pictureBox8.Location = new Point(33, 93);
            pictureBox8.Name = "pictureBox8";
            pictureBox8.Size = new Size(24, 24);
            pictureBox8.TabIndex = 8;
            pictureBox8.TabStop = false;
            pictureBox8.Click += pictureBox8_Click;
            // 
            // pictureBox3
            // 
            pictureBox3.BackColor = Color.FromArgb(0, 192, 0);
            pictureBox3.BorderStyle = BorderStyle.FixedSingle;
            pictureBox3.Location = new Point(3, 93);
            pictureBox3.Name = "pictureBox3";
            pictureBox3.Size = new Size(24, 24);
            pictureBox3.TabIndex = 7;
            pictureBox3.TabStop = false;
            // 
            // pictureBox7
            // 
            pictureBox7.BackColor = Color.Red;
            pictureBox7.BorderStyle = BorderStyle.FixedSingle;
            pictureBox7.Location = new Point(3, 63);
            pictureBox7.Name = "pictureBox7";
            pictureBox7.Size = new Size(24, 24);
            pictureBox7.TabIndex = 6;
            pictureBox7.TabStop = false;
            // 
            // pictureBox6
            // 
            pictureBox6.BackColor = Color.Blue;
            pictureBox6.BorderStyle = BorderStyle.FixedSingle;
            pictureBox6.Location = new Point(33, 63);
            pictureBox6.Name = "pictureBox6";
            pictureBox6.Size = new Size(24, 24);
            pictureBox6.TabIndex = 5;
            pictureBox6.TabStop = false;
            // 
            // pictureBox5
            // 
            pictureBox5.BackColor = Color.Silver;
            pictureBox5.BorderStyle = BorderStyle.FixedSingle;
            pictureBox5.Location = new Point(33, 33);
            pictureBox5.Name = "pictureBox5";
            pictureBox5.Size = new Size(24, 24);
            pictureBox5.TabIndex = 4;
            pictureBox5.TabStop = false;
            // 
            // pictureBox4
            // 
            pictureBox4.BackColor = Color.Gray;
            pictureBox4.BorderStyle = BorderStyle.FixedSingle;
            pictureBox4.Location = new Point(3, 33);
            pictureBox4.Name = "pictureBox4";
            pictureBox4.Size = new Size(24, 24);
            pictureBox4.TabIndex = 3;
            pictureBox4.TabStop = false;
            // 
            // pictureBox2
            // 
            pictureBox2.BackColor = Color.White;
            pictureBox2.BorderStyle = BorderStyle.FixedSingle;
            pictureBox2.Location = new Point(33, 3);
            pictureBox2.Name = "pictureBox2";
            pictureBox2.Size = new Size(24, 24);
            pictureBox2.TabIndex = 1;
            pictureBox2.TabStop = false;
            // 
            // pbColorN
            // 
            pbColorN.BackColor = Color.Black;
            pbColorN.BorderStyle = BorderStyle.FixedSingle;
            pbColorN.Location = new Point(3, 3);
            pbColorN.Name = "pbColorN";
            pbColorN.Size = new Size(24, 24);
            pbColorN.TabIndex = 0;
            pbColorN.TabStop = false;
            pbColorN.Click += pictureBox1_Click;
            pbColorN.MouseDown += pbColorN_MouseDown;
            // 
            // ColorPaletteControl
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            Controls.Add(groupBox1);
            Name = "ColorPaletteControl";
            Size = new Size(150, 297);
            ((System.ComponentModel.ISupportInitialize)pbBgColor).EndInit();
            ((System.ComponentModel.ISupportInitialize)pbSecondaryColor).EndInit();
            ((System.ComponentModel.ISupportInitialize)pbSelectedColor).EndInit();
            groupBox1.ResumeLayout(false);
            pAvailableColors.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)pictureBox8).EndInit();
            ((System.ComponentModel.ISupportInitialize)pictureBox3).EndInit();
            ((System.ComponentModel.ISupportInitialize)pictureBox7).EndInit();
            ((System.ComponentModel.ISupportInitialize)pictureBox6).EndInit();
            ((System.ComponentModel.ISupportInitialize)pictureBox5).EndInit();
            ((System.ComponentModel.ISupportInitialize)pictureBox4).EndInit();
            ((System.ComponentModel.ISupportInitialize)pictureBox2).EndInit();
            ((System.ComponentModel.ISupportInitialize)pbColorN).EndInit();
            ResumeLayout(false);
        }

        #endregion

        private PictureBox pbBgColor;
        private PictureBox pbSecondaryColor;
        private PictureBox pbSelectedColor;
        private GroupBox groupBox1;
        private Panel pAvailableColors;
        private PictureBox pictureBox8;
        private PictureBox pictureBox3;
        private PictureBox pictureBox7;
        private PictureBox pictureBox6;
        private PictureBox pictureBox5;
        private PictureBox pictureBox4;
        private PictureBox pictureBox2;
        private PictureBox pbColorN;
    }
}
