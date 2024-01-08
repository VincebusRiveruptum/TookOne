namespace animationApp {
    partial class Main {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
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
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent() {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Main));
            backgroundWorker1 = new System.ComponentModel.BackgroundWorker();
            menuStrip1 = new MenuStrip();
            editToolStripMenuItem = new ToolStripMenuItem();
            undoToolStripMenuItem = new ToolStripMenuItem();
            redoToolStripMenuItem = new ToolStripMenuItem();
            toolStripMenuItem3 = new ToolStripSeparator();
            cutToolStripMenuItem = new ToolStripMenuItem();
            copyToolStripMenuItem = new ToolStripMenuItem();
            pasteToolStripMenuItem = new ToolStripMenuItem();
            toolStripMenuItem4 = new ToolStripSeparator();
            preferencesToolStripMenuItem = new ToolStripMenuItem();
            fileToolStripMenuItem = new ToolStripMenuItem();
            newToolStripMenuItem = new ToolStripMenuItem();
            toolStripMenuItem7 = new ToolStripSeparator();
            openToolStripMenuItem = new ToolStripMenuItem();
            toolStripMenuItem2 = new ToolStripMenuItem();
            saveToolStripMenuItem = new ToolStripMenuItem();
            toolStripMenuItem1 = new ToolStripSeparator();
            quitToolStripMenuItem = new ToolStripMenuItem();
            viewToolStripMenuItem = new ToolStripMenuItem();
            fillPatternsToolStripMenuItem = new ToolStripMenuItem();
            toolStripMenuItem5 = new ToolStripSeparator();
            eToolStripMenuItem = new ToolStripMenuItem();
            detectUsedColorsToolStripMenuItem = new ToolStripMenuItem();
            toolStripMenuItem6 = new ToolStripSeparator();
            onionSkinningOptionsToolStripMenuItem = new ToolStripMenuItem();
            previousFrameToolStripMenuItem = new ToolStripMenuItem();
            nextFrameToolStripMenuItem = new ToolStripMenuItem();
            helpToolStripMenuItem = new ToolStripMenuItem();
            aboutToolStripMenuItem = new ToolStripMenuItem();
            toolStrip1 = new ToolStrip();
            tsButtonNewFile = new ToolStripButton();
            tsButtonOpen = new ToolStripButton();
            tsButtonSave = new ToolStripButton();
            toolStripSeparator1 = new ToolStripSeparator();
            tsButtonCut = new ToolStripButton();
            tsButtonCopy = new ToolStripButton();
            tsButtonPaste = new ToolStripButton();
            toolStripSeparator2 = new ToolStripSeparator();
            tsButtonUndo = new ToolStripButton();
            tsButtonRedo = new ToolStripButton();
            toolStripSeparator3 = new ToolStripSeparator();
            tsButtonPrevious = new ToolStripButton();
            tsButtonForward = new ToolStripButton();
            toolStripSeparator4 = new ToolStripSeparator();
            tsButtonToggleGrid = new ToolStripButton();
            tsButtonOnionSkinning = new ToolStripButton();
            toolStripSeparator5 = new ToolStripSeparator();
            tsButtonPlay = new ToolStripButton();
            statusStrip1 = new StatusStrip();
            tssInfo = new ToolStripStatusLabel();
            tssMouseCoords = new ToolStripStatusLabel();
            tssToolUsed = new ToolStripStatusLabel();
            mainPictureBox = new PictureBox();
            toolStripContainer1 = new ToolStripContainer();
            panel1 = new Panel();
            toolStrip2 = new ToolStrip();
            tsButtonPencil = new ToolStripButton();
            tsButtonDrawRectangle = new ToolStripButton();
            tsButtonRectangleSelection = new ToolStripButton();
            menuStrip1.SuspendLayout();
            toolStrip1.SuspendLayout();
            statusStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)mainPictureBox).BeginInit();
            toolStripContainer1.ContentPanel.SuspendLayout();
            toolStripContainer1.TopToolStripPanel.SuspendLayout();
            toolStripContainer1.SuspendLayout();
            panel1.SuspendLayout();
            toolStrip2.SuspendLayout();
            SuspendLayout();
            // 
            // menuStrip1
            // 
            menuStrip1.Items.AddRange(new ToolStripItem[] { fileToolStripMenuItem, editToolStripMenuItem, viewToolStripMenuItem, helpToolStripMenuItem });
            menuStrip1.Location = new Point(0, 0);
            menuStrip1.Name = "menuStrip1";
            menuStrip1.RenderMode = ToolStripRenderMode.System;
            menuStrip1.Size = new Size(844, 24);
            menuStrip1.TabIndex = 0;
            menuStrip1.Text = "menuStrip1";
            menuStrip1.ItemClicked += menuStrip1_ItemClicked;
            // 
            // editToolStripMenuItem
            // 
            editToolStripMenuItem.DropDownItems.AddRange(new ToolStripItem[] { undoToolStripMenuItem, redoToolStripMenuItem, toolStripMenuItem3, cutToolStripMenuItem, copyToolStripMenuItem, pasteToolStripMenuItem, toolStripMenuItem4, preferencesToolStripMenuItem });
            editToolStripMenuItem.Name = "editToolStripMenuItem";
            editToolStripMenuItem.Size = new Size(39, 20);
            editToolStripMenuItem.Text = "Edit";
            // 
            // undoToolStripMenuItem
            // 
            undoToolStripMenuItem.Name = "undoToolStripMenuItem";
            undoToolStripMenuItem.Size = new Size(144, 22);
            undoToolStripMenuItem.Text = "Undo";
            // 
            // redoToolStripMenuItem
            // 
            redoToolStripMenuItem.Name = "redoToolStripMenuItem";
            redoToolStripMenuItem.Size = new Size(144, 22);
            redoToolStripMenuItem.Text = "Redo";
            // 
            // toolStripMenuItem3
            // 
            toolStripMenuItem3.Name = "toolStripMenuItem3";
            toolStripMenuItem3.Size = new Size(141, 6);
            // 
            // cutToolStripMenuItem
            // 
            cutToolStripMenuItem.Name = "cutToolStripMenuItem";
            cutToolStripMenuItem.Size = new Size(144, 22);
            cutToolStripMenuItem.Text = "Cut";
            // 
            // copyToolStripMenuItem
            // 
            copyToolStripMenuItem.Name = "copyToolStripMenuItem";
            copyToolStripMenuItem.Size = new Size(144, 22);
            copyToolStripMenuItem.Text = "Copy";
            // 
            // pasteToolStripMenuItem
            // 
            pasteToolStripMenuItem.Name = "pasteToolStripMenuItem";
            pasteToolStripMenuItem.Size = new Size(144, 22);
            pasteToolStripMenuItem.Text = "Paste";
            // 
            // toolStripMenuItem4
            // 
            toolStripMenuItem4.Name = "toolStripMenuItem4";
            toolStripMenuItem4.Size = new Size(141, 6);
            // 
            // preferencesToolStripMenuItem
            // 
            preferencesToolStripMenuItem.Name = "preferencesToolStripMenuItem";
            preferencesToolStripMenuItem.Size = new Size(144, 22);
            preferencesToolStripMenuItem.Text = "Preferences...";
            preferencesToolStripMenuItem.Click += preferencesToolStripMenuItem_Click;
            // 
            // fileToolStripMenuItem
            // 
            fileToolStripMenuItem.DropDownItems.AddRange(new ToolStripItem[] { newToolStripMenuItem, toolStripMenuItem7, openToolStripMenuItem, toolStripMenuItem2, saveToolStripMenuItem, toolStripMenuItem1, quitToolStripMenuItem });
            fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            fileToolStripMenuItem.Size = new Size(37, 20);
            fileToolStripMenuItem.Text = "File";
            fileToolStripMenuItem.Click += fileToolStripMenuItem_Click;
            // 
            // newToolStripMenuItem
            // 
            newToolStripMenuItem.Name = "newToolStripMenuItem";
            newToolStripMenuItem.Size = new Size(180, 22);
            newToolStripMenuItem.Text = "New";
            newToolStripMenuItem.Click += newToolStripMenuItem_Click;
            // 
            // toolStripMenuItem7
            // 
            toolStripMenuItem7.Name = "toolStripMenuItem7";
            toolStripMenuItem7.Size = new Size(177, 6);
            // 
            // openToolStripMenuItem
            // 
            openToolStripMenuItem.Name = "openToolStripMenuItem";
            openToolStripMenuItem.Size = new Size(180, 22);
            openToolStripMenuItem.Text = "Open...";
            openToolStripMenuItem.Click += openToolStripMenuItem_Click;
            // 
            // toolStripMenuItem2
            // 
            toolStripMenuItem2.Name = "toolStripMenuItem2";
            toolStripMenuItem2.Size = new Size(180, 22);
            toolStripMenuItem2.Text = "Save";
            // 
            // saveToolStripMenuItem
            // 
            saveToolStripMenuItem.Name = "saveToolStripMenuItem";
            saveToolStripMenuItem.Size = new Size(180, 22);
            saveToolStripMenuItem.Text = "Save As...";
            saveToolStripMenuItem.Click += saveToolStripMenuItem_Click;
            // 
            // toolStripMenuItem1
            // 
            toolStripMenuItem1.Name = "toolStripMenuItem1";
            toolStripMenuItem1.Size = new Size(177, 6);
            // 
            // quitToolStripMenuItem
            // 
            quitToolStripMenuItem.Name = "quitToolStripMenuItem";
            quitToolStripMenuItem.Size = new Size(180, 22);
            quitToolStripMenuItem.Text = "Exit";
            quitToolStripMenuItem.Click += quitToolStripMenuItem_Click;
            // 
            // viewToolStripMenuItem
            // 
            viewToolStripMenuItem.DropDownItems.AddRange(new ToolStripItem[] { fillPatternsToolStripMenuItem, toolStripMenuItem5, eToolStripMenuItem, detectUsedColorsToolStripMenuItem, toolStripMenuItem6, onionSkinningOptionsToolStripMenuItem });
            viewToolStripMenuItem.Name = "viewToolStripMenuItem";
            viewToolStripMenuItem.Size = new Size(61, 20);
            viewToolStripMenuItem.Text = "&Options";
            viewToolStripMenuItem.Click += viewToolStripMenuItem_Click;
            // 
            // fillPatternsToolStripMenuItem
            // 
            fillPatternsToolStripMenuItem.Name = "fillPatternsToolStripMenuItem";
            fillPatternsToolStripMenuItem.Size = new Size(201, 22);
            fillPatternsToolStripMenuItem.Text = "Fill Patterns";
            fillPatternsToolStripMenuItem.Click += fillPatternsToolStripMenuItem_Click;
            // 
            // toolStripMenuItem5
            // 
            toolStripMenuItem5.Name = "toolStripMenuItem5";
            toolStripMenuItem5.Size = new Size(198, 6);
            // 
            // eToolStripMenuItem
            // 
            eToolStripMenuItem.Name = "eToolStripMenuItem";
            eToolStripMenuItem.Size = new Size(201, 22);
            eToolStripMenuItem.Text = "Edit Palette...";
            eToolStripMenuItem.Click += eToolStripMenuItem_Click;
            // 
            // detectUsedColorsToolStripMenuItem
            // 
            detectUsedColorsToolStripMenuItem.Name = "detectUsedColorsToolStripMenuItem";
            detectUsedColorsToolStripMenuItem.Size = new Size(201, 22);
            detectUsedColorsToolStripMenuItem.Text = "Detect used colors...";
            detectUsedColorsToolStripMenuItem.Click += detectUsedColorsToolStripMenuItem_Click;
            // 
            // toolStripMenuItem6
            // 
            toolStripMenuItem6.Name = "toolStripMenuItem6";
            toolStripMenuItem6.Size = new Size(198, 6);
            // 
            // onionSkinningOptionsToolStripMenuItem
            // 
            onionSkinningOptionsToolStripMenuItem.DropDownItems.AddRange(new ToolStripItem[] { previousFrameToolStripMenuItem, nextFrameToolStripMenuItem });
            onionSkinningOptionsToolStripMenuItem.Name = "onionSkinningOptionsToolStripMenuItem";
            onionSkinningOptionsToolStripMenuItem.Size = new Size(201, 22);
            onionSkinningOptionsToolStripMenuItem.Text = "Onion Skinning Options";
            // 
            // previousFrameToolStripMenuItem
            // 
            previousFrameToolStripMenuItem.Name = "previousFrameToolStripMenuItem";
            previousFrameToolStripMenuItem.Size = new Size(155, 22);
            previousFrameToolStripMenuItem.Text = "Previous Frame";
            // 
            // nextFrameToolStripMenuItem
            // 
            nextFrameToolStripMenuItem.Name = "nextFrameToolStripMenuItem";
            nextFrameToolStripMenuItem.Size = new Size(155, 22);
            nextFrameToolStripMenuItem.Text = "Next Frame";
            // 
            // helpToolStripMenuItem
            // 
            helpToolStripMenuItem.DropDownItems.AddRange(new ToolStripItem[] { aboutToolStripMenuItem });
            helpToolStripMenuItem.Name = "helpToolStripMenuItem";
            helpToolStripMenuItem.Size = new Size(44, 20);
            helpToolStripMenuItem.Text = "Help";
            // 
            // aboutToolStripMenuItem
            // 
            aboutToolStripMenuItem.Name = "aboutToolStripMenuItem";
            aboutToolStripMenuItem.Size = new Size(107, 22);
            aboutToolStripMenuItem.Text = "About";
            aboutToolStripMenuItem.Click += aboutToolStripMenuItem_Click;
            // 
            // toolStrip1
            // 
            toolStrip1.Dock = DockStyle.None;
            toolStrip1.Items.AddRange(new ToolStripItem[] { tsButtonNewFile, tsButtonOpen, tsButtonSave, toolStripSeparator1, tsButtonCut, tsButtonCopy, tsButtonPaste, toolStripSeparator2, tsButtonUndo, tsButtonRedo, toolStripSeparator3, tsButtonPrevious, tsButtonForward, toolStripSeparator4, tsButtonToggleGrid, tsButtonOnionSkinning, toolStripSeparator5, tsButtonPlay });
            toolStrip1.Location = new Point(3, 0);
            toolStrip1.Name = "toolStrip1";
            toolStrip1.Size = new Size(341, 25);
            toolStrip1.TabIndex = 1;
            toolStrip1.Text = "toolStrip1";
            toolStrip1.ItemClicked += toolStrip1_ItemClicked;
            // 
            // tsButtonNewFile
            // 
            tsButtonNewFile.DisplayStyle = ToolStripItemDisplayStyle.Image;
            tsButtonNewFile.Image = (Image)resources.GetObject("tsButtonNewFile.Image");
            tsButtonNewFile.ImageTransparentColor = Color.Magenta;
            tsButtonNewFile.Name = "tsButtonNewFile";
            tsButtonNewFile.Size = new Size(23, 22);
            tsButtonNewFile.Text = "New...";
            tsButtonNewFile.Click += toolStripButton1_Click;
            tsButtonNewFile.MouseLeave += tsButtonNewFile_MouseLeave;
            tsButtonNewFile.MouseHover += tsButtonNewFile_MouseHover;
            // 
            // tsButtonOpen
            // 
            tsButtonOpen.DisplayStyle = ToolStripItemDisplayStyle.Image;
            tsButtonOpen.Image = (Image)resources.GetObject("tsButtonOpen.Image");
            tsButtonOpen.ImageTransparentColor = Color.Magenta;
            tsButtonOpen.Name = "tsButtonOpen";
            tsButtonOpen.Size = new Size(23, 22);
            tsButtonOpen.Text = "Open File";
            tsButtonOpen.ToolTipText = "Open File...";
            tsButtonOpen.Click += tsButtonOpen_Click;
            // 
            // tsButtonSave
            // 
            tsButtonSave.DisplayStyle = ToolStripItemDisplayStyle.Image;
            tsButtonSave.Image = (Image)resources.GetObject("tsButtonSave.Image");
            tsButtonSave.ImageTransparentColor = Color.Magenta;
            tsButtonSave.Name = "tsButtonSave";
            tsButtonSave.Size = new Size(23, 22);
            tsButtonSave.Text = "Save File";
            tsButtonSave.Click += tsButtonSave_Click;
            // 
            // toolStripSeparator1
            // 
            toolStripSeparator1.Name = "toolStripSeparator1";
            toolStripSeparator1.Size = new Size(6, 25);
            // 
            // tsButtonCut
            // 
            tsButtonCut.DisplayStyle = ToolStripItemDisplayStyle.Image;
            tsButtonCut.Image = (Image)resources.GetObject("tsButtonCut.Image");
            tsButtonCut.ImageTransparentColor = Color.Magenta;
            tsButtonCut.Name = "tsButtonCut";
            tsButtonCut.Size = new Size(23, 22);
            tsButtonCut.Text = "Cut";
            tsButtonCut.Click += tsButtonCut_Click;
            // 
            // tsButtonCopy
            // 
            tsButtonCopy.DisplayStyle = ToolStripItemDisplayStyle.Image;
            tsButtonCopy.Image = (Image)resources.GetObject("tsButtonCopy.Image");
            tsButtonCopy.ImageTransparentColor = Color.Magenta;
            tsButtonCopy.Name = "tsButtonCopy";
            tsButtonCopy.Size = new Size(23, 22);
            tsButtonCopy.Text = "Copy";
            tsButtonCopy.ToolTipText = "Copy";
            // 
            // tsButtonPaste
            // 
            tsButtonPaste.DisplayStyle = ToolStripItemDisplayStyle.Image;
            tsButtonPaste.Image = (Image)resources.GetObject("tsButtonPaste.Image");
            tsButtonPaste.ImageTransparentColor = Color.Magenta;
            tsButtonPaste.Name = "tsButtonPaste";
            tsButtonPaste.Size = new Size(23, 22);
            tsButtonPaste.Text = "Paste";
            // 
            // toolStripSeparator2
            // 
            toolStripSeparator2.Name = "toolStripSeparator2";
            toolStripSeparator2.Size = new Size(6, 25);
            // 
            // tsButtonUndo
            // 
            tsButtonUndo.DisplayStyle = ToolStripItemDisplayStyle.Image;
            tsButtonUndo.Image = (Image)resources.GetObject("tsButtonUndo.Image");
            tsButtonUndo.ImageTransparentColor = Color.Magenta;
            tsButtonUndo.Name = "tsButtonUndo";
            tsButtonUndo.Size = new Size(23, 22);
            tsButtonUndo.Text = "Undo";
            tsButtonUndo.Click += tsButtonUndo_Click;
            // 
            // tsButtonRedo
            // 
            tsButtonRedo.DisplayStyle = ToolStripItemDisplayStyle.Image;
            tsButtonRedo.Image = (Image)resources.GetObject("tsButtonRedo.Image");
            tsButtonRedo.ImageTransparentColor = Color.Magenta;
            tsButtonRedo.Name = "tsButtonRedo";
            tsButtonRedo.Size = new Size(23, 22);
            tsButtonRedo.Text = "Redo";
            tsButtonRedo.Click += tsButtonRedo_Click;
            // 
            // toolStripSeparator3
            // 
            toolStripSeparator3.Name = "toolStripSeparator3";
            toolStripSeparator3.Size = new Size(6, 25);
            // 
            // tsButtonPrevious
            // 
            tsButtonPrevious.DisplayStyle = ToolStripItemDisplayStyle.Image;
            tsButtonPrevious.Image = (Image)resources.GetObject("tsButtonPrevious.Image");
            tsButtonPrevious.ImageTransparentColor = Color.Magenta;
            tsButtonPrevious.Name = "tsButtonPrevious";
            tsButtonPrevious.Size = new Size(23, 22);
            tsButtonPrevious.Text = "Previous Frame";
            // 
            // tsButtonForward
            // 
            tsButtonForward.DisplayStyle = ToolStripItemDisplayStyle.Image;
            tsButtonForward.Image = (Image)resources.GetObject("tsButtonForward.Image");
            tsButtonForward.ImageTransparentColor = Color.Magenta;
            tsButtonForward.Name = "tsButtonForward";
            tsButtonForward.Size = new Size(23, 22);
            tsButtonForward.Text = "Forward Frame";
            tsButtonForward.Click += toolStripButton2_Click;
            // 
            // toolStripSeparator4
            // 
            toolStripSeparator4.Name = "toolStripSeparator4";
            toolStripSeparator4.Size = new Size(6, 25);
            // 
            // tsButtonToggleGrid
            // 
            tsButtonToggleGrid.DisplayStyle = ToolStripItemDisplayStyle.Image;
            tsButtonToggleGrid.Image = (Image)resources.GetObject("tsButtonToggleGrid.Image");
            tsButtonToggleGrid.ImageTransparentColor = Color.Magenta;
            tsButtonToggleGrid.Name = "tsButtonToggleGrid";
            tsButtonToggleGrid.Size = new Size(23, 22);
            tsButtonToggleGrid.Text = "Toggle Grid";
            // 
            // tsButtonOnionSkinning
            // 
            tsButtonOnionSkinning.DisplayStyle = ToolStripItemDisplayStyle.Image;
            tsButtonOnionSkinning.Image = (Image)resources.GetObject("tsButtonOnionSkinning.Image");
            tsButtonOnionSkinning.ImageTransparentColor = Color.Magenta;
            tsButtonOnionSkinning.Name = "tsButtonOnionSkinning";
            tsButtonOnionSkinning.Size = new Size(23, 22);
            tsButtonOnionSkinning.Text = "Toggle Onion Skinning On/Off";
            // 
            // toolStripSeparator5
            // 
            toolStripSeparator5.Name = "toolStripSeparator5";
            toolStripSeparator5.Size = new Size(6, 25);
            // 
            // tsButtonPlay
            // 
            tsButtonPlay.DisplayStyle = ToolStripItemDisplayStyle.Image;
            tsButtonPlay.Image = (Image)resources.GetObject("tsButtonPlay.Image");
            tsButtonPlay.ImageTransparentColor = Color.Magenta;
            tsButtonPlay.Name = "tsButtonPlay";
            tsButtonPlay.Size = new Size(23, 22);
            tsButtonPlay.Text = "Play Animation";
            // 
            // statusStrip1
            // 
            statusStrip1.Items.AddRange(new ToolStripItem[] { tssInfo, tssMouseCoords, tssToolUsed });
            statusStrip1.Location = new Point(0, 570);
            statusStrip1.Name = "statusStrip1";
            statusStrip1.RenderMode = ToolStripRenderMode.ManagerRenderMode;
            statusStrip1.Size = new Size(844, 22);
            statusStrip1.TabIndex = 2;
            statusStrip1.Text = "statusStrip1";
            // 
            // tssInfo
            // 
            tssInfo.Name = "tssInfo";
            tssInfo.Size = new Size(57, 17);
            tssInfo.Text = "Welcome";
            // 
            // tssMouseCoords
            // 
            tssMouseCoords.BorderStyle = Border3DStyle.Bump;
            tssMouseCoords.Name = "tssMouseCoords";
            tssMouseCoords.Size = new Size(43, 17);
            tssMouseCoords.Text = "Mouse";
            // 
            // tssToolUsed
            // 
            tssToolUsed.Name = "tssToolUsed";
            tssToolUsed.Size = new Size(29, 17);
            tssToolUsed.Text = "Tool";
            // 
            // mainPictureBox
            // 
            mainPictureBox.BackColor = SystemColors.AppWorkspace;
            mainPictureBox.BackgroundImageLayout = ImageLayout.Center;
            mainPictureBox.BorderStyle = BorderStyle.FixedSingle;
            mainPictureBox.Cursor = Cursors.Cross;
            mainPictureBox.Location = new Point(5, 5);
            mainPictureBox.Name = "mainPictureBox";
            mainPictureBox.Size = new Size(0, 0);
            mainPictureBox.TabIndex = 3;
            mainPictureBox.TabStop = false;
            mainPictureBox.Click += mainPictureBox_Click;
            mainPictureBox.Paint += mainPictureBox_Paint;
            mainPictureBox.MouseDown += mainPictureBox_MouseDown;
            mainPictureBox.MouseMove += mainPictureBox_MouseMove;
            mainPictureBox.MouseUp += mainPictureBox_MouseUp;
            // 
            // toolStripContainer1
            // 
            // 
            // toolStripContainer1.BottomToolStripPanel
            // 
            toolStripContainer1.BottomToolStripPanel.Click += toolStripContainer1_BottomToolStripPanel_Click;
            // 
            // toolStripContainer1.ContentPanel
            // 
            toolStripContainer1.ContentPanel.Controls.Add(panel1);
            toolStripContainer1.ContentPanel.Size = new Size(844, 496);
            toolStripContainer1.Dock = DockStyle.Fill;
            toolStripContainer1.Location = new Point(0, 24);
            toolStripContainer1.Name = "toolStripContainer1";
            toolStripContainer1.Size = new Size(844, 546);
            toolStripContainer1.TabIndex = 6;
            toolStripContainer1.Text = "toolStripContainer1";
            // 
            // toolStripContainer1.TopToolStripPanel
            // 
            toolStripContainer1.TopToolStripPanel.Controls.Add(toolStrip1);
            toolStripContainer1.TopToolStripPanel.Controls.Add(toolStrip2);
            // 
            // panel1
            // 
            panel1.AutoScroll = true;
            panel1.BackColor = SystemColors.AppWorkspace;
            panel1.Controls.Add(mainPictureBox);
            panel1.Dock = DockStyle.Fill;
            panel1.Location = new Point(0, 0);
            panel1.Margin = new Padding(0);
            panel1.Name = "panel1";
            panel1.Size = new Size(844, 496);
            panel1.TabIndex = 4;
            panel1.SizeChanged += panel1_SizeChanged;
            panel1.Paint += panel1_Paint;
            // 
            // toolStrip2
            // 
            toolStrip2.Dock = DockStyle.None;
            toolStrip2.Items.AddRange(new ToolStripItem[] { tsButtonPencil, tsButtonDrawRectangle, tsButtonRectangleSelection });
            toolStrip2.Location = new Point(4, 25);
            toolStrip2.Name = "toolStrip2";
            toolStrip2.Size = new Size(81, 25);
            toolStrip2.TabIndex = 5;
            toolStrip2.Text = "toolStrip2";
            toolStrip2.ItemClicked += toolStrip2_ItemClicked;
            // 
            // tsButtonPencil
            // 
            tsButtonPencil.CheckOnClick = true;
            tsButtonPencil.DisplayStyle = ToolStripItemDisplayStyle.Image;
            tsButtonPencil.Image = (Image)resources.GetObject("tsButtonPencil.Image");
            tsButtonPencil.ImageTransparentColor = Color.Magenta;
            tsButtonPencil.Name = "tsButtonPencil";
            tsButtonPencil.Size = new Size(23, 22);
            tsButtonPencil.Text = "tsPencilButton";
            tsButtonPencil.ToolTipText = "Pencil";
            tsButtonPencil.CheckedChanged += tsButtonPencil_CheckedChanged;
            tsButtonPencil.Click += tsButtonPencil_Click;
            // 
            // tsButtonDrawRectangle
            // 
            tsButtonDrawRectangle.CheckOnClick = true;
            tsButtonDrawRectangle.DisplayStyle = ToolStripItemDisplayStyle.Image;
            tsButtonDrawRectangle.Image = (Image)resources.GetObject("tsButtonDrawRectangle.Image");
            tsButtonDrawRectangle.ImageTransparentColor = Color.Magenta;
            tsButtonDrawRectangle.Name = "tsButtonDrawRectangle";
            tsButtonDrawRectangle.Size = new Size(23, 22);
            tsButtonDrawRectangle.Text = "Draw Rectangle";
            tsButtonDrawRectangle.Click += toolStripButton2_Click_1;
            // 
            // tsButtonRectangleSelection
            // 
            tsButtonRectangleSelection.CheckOnClick = true;
            tsButtonRectangleSelection.DisplayStyle = ToolStripItemDisplayStyle.Image;
            tsButtonRectangleSelection.Image = (Image)resources.GetObject("tsButtonRectangleSelection.Image");
            tsButtonRectangleSelection.ImageTransparentColor = Color.Magenta;
            tsButtonRectangleSelection.Name = "tsButtonRectangleSelection";
            tsButtonRectangleSelection.Size = new Size(23, 22);
            tsButtonRectangleSelection.Text = "Rectangular Selection";
            tsButtonRectangleSelection.Click += toolStripButton1_Click_1;
            // 
            // Main
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(844, 592);
            Controls.Add(toolStripContainer1);
            Controls.Add(statusStrip1);
            Controls.Add(menuStrip1);
            MainMenuStrip = menuStrip1;
            Name = "Main";
            Text = "Form1";
            Load += Main_Load;
            DpiChanged += Main_DpiChanged;
            menuStrip1.ResumeLayout(false);
            menuStrip1.PerformLayout();
            toolStrip1.ResumeLayout(false);
            toolStrip1.PerformLayout();
            statusStrip1.ResumeLayout(false);
            statusStrip1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)mainPictureBox).EndInit();
            toolStripContainer1.ContentPanel.ResumeLayout(false);
            toolStripContainer1.TopToolStripPanel.ResumeLayout(false);
            toolStripContainer1.TopToolStripPanel.PerformLayout();
            toolStripContainer1.ResumeLayout(false);
            toolStripContainer1.PerformLayout();
            panel1.ResumeLayout(false);
            toolStrip2.ResumeLayout(false);
            toolStrip2.PerformLayout();
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private System.ComponentModel.BackgroundWorker backgroundWorker1;
        private MenuStrip menuStrip1;
        private ToolStripMenuItem fileToolStripMenuItem;
        private ToolStripMenuItem quitToolStripMenuItem;
        private ToolStripMenuItem editToolStripMenuItem;
        private ToolStripMenuItem helpToolStripMenuItem;
        private ToolStripMenuItem aboutToolStripMenuItem;
        private ToolStrip toolStrip1;
        private ToolStripButton tsButtonNewFile;
        private StatusStrip statusStrip1;
        private ToolStripMenuItem openToolStripMenuItem;
        private ToolStripSeparator toolStripMenuItem1;
        private ToolStripMenuItem newToolStripMenuItem;
        private ToolStripMenuItem saveToolStripMenuItem;
        private ToolStripMenuItem toolStripMenuItem2;
        private PictureBox mainPictureBox;
        private ToolStripSeparator toolStripSeparator1;
        private ToolStripButton tsButtonOpen;
        private ToolStripButton tsButtonSave;
        private ToolStripButton tsButtonCut;
        private ToolStripButton tsButtonCopy;
        private ToolStripButton tsButtonPaste;
        private ToolStripSeparator toolStripSeparator2;
        private ToolStripButton tsButtonRedo;
        private ToolStripSeparator toolStripSeparator3;
        private ToolStripButton tsButtonPrevious;
        private ToolStripButton tsButtonForward;
        private ToolStripSeparator toolStripSeparator4;
        private ToolStripButton tsButtonToggleGrid;
        private ToolStripButton tsButtonOnionSkinning;
        private ToolStripSeparator toolStripSeparator5;
        private ToolStripButton tsButtonPlay;
        private ToolStripContainer toolStripContainer1;
        private ToolStripButton tsButtonUndo;
        private ToolStrip toolStrip2;
        private ToolStripButton tsButtonPencil;
        private ToolStripButton tsButtonDrawRectangle;
        private ToolStripButton tsButtonRectangleSelection;
        private ToolStripStatusLabel tssInfo;
        private ToolStripMenuItem undoToolStripMenuItem;
        private ToolStripMenuItem redoToolStripMenuItem;
        private ToolStripSeparator toolStripMenuItem3;
        private ToolStripMenuItem cutToolStripMenuItem;
        private ToolStripMenuItem copyToolStripMenuItem;
        private ToolStripMenuItem pasteToolStripMenuItem;
        private ToolStripSeparator toolStripMenuItem4;
        private ToolStripMenuItem preferencesToolStripMenuItem;
        private ToolStripMenuItem viewToolStripMenuItem;
        private ToolStripMenuItem fillPatternsToolStripMenuItem;
        private ToolStripSeparator toolStripMenuItem5;
        private ToolStripMenuItem eToolStripMenuItem;
        private ToolStripMenuItem detectUsedColorsToolStripMenuItem;
        private ToolStripSeparator toolStripMenuItem6;
        private ToolStripMenuItem onionSkinningOptionsToolStripMenuItem;
        private ToolStripMenuItem previousFrameToolStripMenuItem;
        private ToolStripMenuItem nextFrameToolStripMenuItem;
        private ToolStripSeparator toolStripMenuItem7;
        private ToolStripStatusLabel tssMouseCoords;
        private ToolStripStatusLabel tssToolUsed;
        private Panel panel1;
    }
}
