using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace animationApp {
    public class AppProperties {
        private bool centeredWorkspace;
        private int zoomScalePosition;
        private float[] zoomScales;
        public AppProperties() {
            this.centeredWorkspace = false;
            this.zoomScalePosition = 2;
            this.zoomScales = new float[] { 0.25f, 0.5f, 1f, 2f, 4f};
        }

        public void setCenteredWorkspace(bool centeredWorkspace) {
            this.centeredWorkspace = centeredWorkspace;
        }

        public bool getCenteredWorkspace() {
            return this.centeredWorkspace;                
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
    }
}
