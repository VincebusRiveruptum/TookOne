using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;


namespace animationApp {
    public class AppProperties {
        private bool centeredWorkspace;
        private int zoomScalePosition;
        private float[] zoomScales;

        private Color selectedColor;
        private Color secondaryColor;
        private Color bgColor;

        private Pen pencilTool;
        private Pen brushTool;

        private bool dockedColors;

        public AppProperties() {
            this.centeredWorkspace = false;
            this.zoomScalePosition = 2;
            this.zoomScales = new float[] { 0.25f, 0.5f, 1f, 2f, 4f};
            this.selectedColor = Color.Black;
            this.secondaryColor = Color.White;
            this.bgColor = Color.Transparent;
            this.dockedColors = true;

            this.pencilTool = new Pen(selectedColor, 1);
            this.brushTool = new Pen(selectedColor, 4);
        }
        /*
        public void setDockedColors(Main main) {
            DockPanel dockPanel = new DockPanel();
            DockContent dockContent = new DockContent();
            ColorPaletteControl paletteControl = new ColorPaletteControl();

            // Docked to form main
            dockPanel.Dock = DockStyle.Fill;
            dockPanel.Theme = new VS2015LightTheme();
            main.Controls.Add(dockPanel);

                // Docked window
                dockPanel.Controls.Add(paletteControl);

                // Dockable window
                dockContent.Text = "Dockable Window";
                dockContent.Show(dockPanel, DockState.DockRight);
                //paletteControl.Dock = DockStyle.Fill;
                //dockContent.Controls.Add(this.toolStripContainer1);
           

        }
        */
        /*
        public DockPanel getDockPanel() {
            return dockPanel;
        }

        public void setDockPanel(DockPanel dockPanel) { 
            this.dockPanel = dockPanel;        
        }

        public ColorPaletteControl getColorPaletteControl() {
            return paletteControl;
        }

        public void setColorPaletteControl(ColorPaletteControl cpc) {
            paletteControl = cpc;
        }
        */

        public Pen getPencilTool() {
            return this.pencilTool;
        }

        public Pen getBrushTool() {
            return this.brushTool;
        }
    
        public void setPencilTool(Pen pencilTool) {
            this.pencilTool = pencilTool;
        }

        public void setBrushTool(Pen brushTool) { 
            this.brushTool = brushTool;
        }
        public void setPencilColor(Color color) {
            this.pencilTool.Color = color;
        }

        public void setBrushToolColor(Color color) {
            this.brushTool.Color = color;
        }

        public Color getPencilToolColor() {
            return this.pencilTool.Color;
        }

        public Color getBrushToolColor() {
            return this.brushTool.Color;
        }
        public bool isDockedColors() {
            return this.dockedColors;
        }

        public void setDockedColors(bool dockedColors) {
            this.dockedColors = dockedColors;
        }

        public void setCenteredWorkspace(bool centeredWorkspace) {
            this.centeredWorkspace = centeredWorkspace;
        }

        public bool getCenteredWorkspace() {
            return this.centeredWorkspace;                
        }

        public float getZoomScaleSelectedValue() {
            return this.zoomScales[this.zoomScalePosition];
        }

        public int getZoomScalePosition() { 
            return this.zoomScalePosition;
        }
        public void setZoomScalePosition(int zoomScalePosition) {
               this.zoomScalePosition = zoomScalePosition;
        }

        public float getZoomScale(int pos) {
            if((0 <= pos) && (pos < this.zoomScales.Length)) {
                return this.zoomScales[pos];
            } else {
                return 1f;
            }
        }

        public int getZoomScalesLength() {
            return this.zoomScales.Length - 1; 
        }

        public void resetProperties() {
            this.centeredWorkspace= false;
            this.zoomScalePosition = 2;
        }

        // Color methods
        public void setSelectedColor(Color color) {
            this.selectedColor = color;
            this.pencilTool.Color = color;
            this.brushTool.Color = color;
        }

        public void setSecondaryColor(Color secondaryColor) { 
            this.secondaryColor = secondaryColor;        
            
        }

        public void setBgColor(Color bgColor) {
            this.bgColor = bgColor;
        }
        public Color getSelectedColor() {
            return this.selectedColor;
        }
        public Color getSecondaryColor() {
            return this.secondaryColor;
        }

        public Color getBgColor() {
            return this.bgColor;
        }
    }
}
