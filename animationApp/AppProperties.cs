using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace animationApp {
    public class AppProperties {
        private bool centeredWorkspace;
        public AppProperties() {
            this.centeredWorkspace = false;
        }

        public void setCenteredWorkspace(bool centeredWorkspace) {
            this.centeredWorkspace = centeredWorkspace;
        }

        public bool getCenteredWorkspace() {
            return this.centeredWorkspace;                
        }
    }
}
