using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Runtime.InteropServices;
using System.Security;
using System.Windows.Forms;

namespace animationApp {
    public partial class Main : Form {


        private void menuStrip1_ItemClicked(object sender, ToolStripItemClickedEventArgs e) {

        }

        private void quitToolStripMenuItem_Click(object sender, EventArgs e) {

        }

        private void aboutToolStripMenuItem_Click(object sender, EventArgs e) {
            openAboutDialog();
        }


        private void openToolStripMenuItem_Click(object sender, EventArgs e) {
            openFile();
        }

        private void textBox1_TextChanged(object sender, EventArgs e) {

        }

        private void Main_Load(object sender, EventArgs e) {
            this.Text = ("Vincebus's app test");

            // Behaviour if no document has been opened
            lockToolStripItems();
        }


        private void saveToolStripMenuItem_Click(object sender, EventArgs e) {
            saveFile();
        }

        private void newToolStripMenuItem_Click(object sender, EventArgs e) {
            newFile();
        }

        private void toolStrip1_ItemClicked(object sender, ToolStripItemClickedEventArgs e) {

        }

        private void toolStripButton1_Click(object sender, EventArgs e) {
            newFile();
        }

        private void mainPictureBox_Click(object sender, EventArgs e) {

        }

        private void tsButtonCut_Click(object sender, EventArgs e) {

        }

        private void toolStripButton2_Click(object sender, EventArgs e) {

        }

        private void fileToolStripMenuItem_Click(object sender, EventArgs e) {

        }

        private void Main_DpiChanged(object sender, DpiChangedEventArgs e) {

        }

        private void tsButtonOpen_Click(object sender, EventArgs e) {
            openFile();
        }

        private void tsButtonSave_Click(object sender, EventArgs e) {
            saveFile();
        }

        private void mainPictureBox_Paint(object sender, PaintEventArgs e) {
            /*
            Graphics g = e.Graphics;
            g.DrawString("This string follows the cursor!", new Font("Arial", 10), Brushes.Blue, new Point(30, 30));
            g.DrawLine(Pens.Red, mainPictureBox.Left, mainPictureBox.Top, mainPictureBox.Right, mainPictureBox.Bottom);
            */
        }

        private void mainPictureBox_MouseMove(object sender, MouseEventArgs e) {

        }

        private void mainPictureBox_MouseDown(object sender, MouseEventArgs e) {


        }

        private void mainPictureBox_MouseUp(object sender, MouseEventArgs e) {

        }

        private void toolStripContainer1_TopToolStripPanel_Click(object sender, EventArgs e) {

        }

        private void toolStripContainer2_TopToolStripPanel_Click(object sender, EventArgs e) {

        }

        private void toolStripContainer1_BottomToolStripPanel_Click(object sender, EventArgs e) {

        }

        private void tsButtonNewFile_MouseHover(object sender, EventArgs e) {
            tssInfo.Text = "Creates a new file";
        }

        private void tsButtonNewFile_MouseLeave(object sender, EventArgs e) {
            tssInfo.Text = "";
        }

        private void viewToolStripMenuItem_Click(object sender, EventArgs e) {

        }

        private void detectUsedColorsToolStripMenuItem_Click(object sender, EventArgs e) {

        }

        private void toolStrip2_ItemClicked(object sender, ToolStripItemClickedEventArgs e) {

        }

        private void tsButtonPencil_CheckedChanged(object sender, EventArgs e) {

        }

        private void tsButtonPencil_Click(object sender, EventArgs e) {
            foreach(ToolStripButton item in ((ToolStripButton)sender).GetCurrentParent().Items) {
                if(item == sender) {
                    item.Checked = true;
                    selectedTool = 0;
                }
                if((item != null) && (item != sender)) {
                    item.Checked = false;
                }
            }
        }

        private void toolStripButton2_Click_1(object sender, EventArgs e) {
            foreach(ToolStripButton item in ((ToolStripButton)sender).GetCurrentParent().Items) {
                if(item == sender) {
                    item.Checked = true;
                    selectedTool = 2;
                }
                if((item != null) && (item != sender)) {
                    item.Checked = false;

                }
            }
        }

        private void toolStripButton1_Click_1(object sender, EventArgs e) {
            foreach(ToolStripButton item in ((ToolStripButton)sender).GetCurrentParent().Items) {
                if(item == sender) {
                    item.Checked = true;
                    selectedTool = 3;
                }
                if((item != null) && (item != sender)) {
                    item.Checked = false;
                }
            }
        }

        private void toolStripStatusLabel2_Click(object sender, EventArgs e) {

        }

        private void tsButtonUndo_Click(object sender, EventArgs e) {
            undo();
        }

        private void tsButtonRedo_Click(object sender, EventArgs e) {
            redo();
        }

        private void fillPatternsToolStripMenuItem_Click(object sender, EventArgs e) {
            FillPatternDialog fillPatternDialog = new FillPatternDialog();
            fillPatternDialog.ShowDialog();
        }

        private void eToolStripMenuItem_Click(object sender, EventArgs e) {

        }

        private void panel1_Paint(object sender, PaintEventArgs e) {

        }

        private void preferencesToolStripMenuItem_Click(object sender, EventArgs e) {
            PreferencesDialog preferencesDialog = new PreferencesDialog(this);
            preferencesDialog.ShowDialog();
        }

        private void panel1_SizeChanged(object sender, EventArgs e) {
            if(appProperties != null) {
                if(appProperties.getCenteredWorkspace() == true) {
                    mainPictureBox.Location = new Point((int)(panel1.Width / 2) - (mainPictureBox.Width / 2), (int)(panel1.Height / 2) - (mainPictureBox.Height / 2));
                }

            }
        }

        private void pbEditorLayer_Paint(object sender, PaintEventArgs e) {
            editorLayer = e.Graphics;
            Pen pen = new Pen(Color.Red);
            editorLayer.DrawRectangle(pen, new Rectangle(new Point(10, 10), new Size(10, 10)));

            /*
            if(appProperties.gridEnabled() == true) {
                drawGrid();
            }

            if(appProperties.rulerEnabled() == true) {
                drawRuler();
            }
            */

        }

        private void pbEditorLayer_MouseDown(object sender, MouseEventArgs e) {
            //We save the current state to the undo history.
            if(this.isDrawing == false) {
                editor.pushUndoState(new Bitmap(mainPictureBox.Image));
                checkUndoRedo();
            }

            this.isDrawing = true;
            this.initialPoint = e.Location;


            statusStrip1.Items["tssToolUsed"].Text = "You are using tool " + selectedTool + ".";
        }

        private void pbEditorLayer_MouseLeave(object sender, EventArgs e) {

        }

        private void pbEditorLayer_MouseMove(object sender, MouseEventArgs e) {
            /*
            int x = e.X;
            int y = e.Y;

            Graphics g = mainPictureBox.CreateGraphics();
            g.DrawString("Hello World!", new Font("Arial", 12), Brushes.Black, x, y);
            */

            if(e.X < editor.getWidth() && e.Y < editor.getHeight()) {
                statusStrip1.Items["tssMouseCoords"].Text = "(X:" + e.X + "," + "Y:" + e.Y + ")";

            }

            switch(this.selectedTool) {
                case 0: {
                        pencilTool(sender, e);
                    }
                    break;
                case 1: {
                        drawLineTool(sender, e);
                    }
                    break;
                case 2: {
                        drawRectangleTool(sender, e);
                    }
                    break;
                case 3: {
                        bucketToolHover();
                    }
                    break;

            }
        }

        public void bucketToolHover() {
            // Put on statusbar an message if the region to fill is open
            return;
        }

        private void pbEditorLayer_MouseUp(object sender, MouseEventArgs e) {

            if(this.isDrawing == true) {
                editor.pushRedoState(new Bitmap(mainPictureBox.Image));
                checkUndoRedo();
            }

            this.isDrawing = false;

            switch(this.selectedTool) {
                case 0: {
                        //pencil tool method NOT NECESSARY
                        //transferToWorkspace();
                    }
                    break;
                case 1: {
                        drawLineToWorkspace(sender, e);
                    }
                    break;
                case 2: {
                        //drawRectangleTool(sender, e);
                        drawRectangleToWorkspace(sender, e);
                    }
                    break;
           
                case 4: {
                        bucketTool(sender, e, appProperties.getSelectedColor());
                    }
                    break;
            }

            statusStrip1.Items["tssToolUsed"].Text = "You finished using tool " + selectedTool + ".";

        }

        private void toolStripButton1_Click_2(object sender, EventArgs e) {
            foreach(ToolStripButton item in ((ToolStripButton)sender).GetCurrentParent().Items) {
                if(item == sender) {
                    item.Checked = true;
                    selectedTool = 1;
                }
                if((item != null) && (item != sender)) {
                    item.Checked = false;

                }
            }
        }

        private void toolStripButton1_Click_3(object sender, EventArgs e) {
            foreach(ToolStripButton item in ((ToolStripButton)sender).GetCurrentParent().Items) {
                if(item == sender) {
                    item.Checked = true;
                    selectedTool = 4;
                }
                if((item != null) && (item != sender)) {
                    item.Checked = false;

                }
            }
        }

        private void toolStripContainer1_TopToolStripPanel_Click_1(object sender, EventArgs e) {

        }

        private void tsButtonZoomOut_Click(object sender, EventArgs e) {
            zoomOut();
        }


        private void tsButtonZoomIn_Click(object sender, EventArgs e) {
            zoomIn();
        }

        private void undoToolStripMenuItem_Click(object sender, EventArgs e) {
            undo();
        }

        private void redoToolStripMenuItem_Click(object sender, EventArgs e) {
            redo();
        }

        private void toolStripContainer1_LeftToolStripPanel_Click(object sender, EventArgs e) {

        }

        private void toolStripContainer1_ContentPanel_Load(object sender, EventArgs e) {

        }

        private void panel2_Paint(object sender, PaintEventArgs e) {

        }

        private void pbColorN_Click(object sender, EventArgs e) {

        }

        private void pbColorN_MouseDown(object sender, MouseEventArgs e) {
            if(e.Button == MouseButtons.Left) {
                appProperties.setSelectedColor(pbColorN.BackColor);
                pbSelectedColor.BackColor = pbColorN.BackColor;
            } 

            if(e.Button == MouseButtons.Right) {
                appProperties.setSecondaryColor(pbColorN.BackColor);
                pbSecondaryColor.BackColor = pbColorN.BackColor;
            }
        }
    }
}
