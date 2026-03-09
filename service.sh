!/bin/bash

# --- Colors ---
G='\033[0;32m' # Green
DG='\033[1;32m' # Bright Green
P='\033[0;35m' # Purple
C='\033[0;36m' # Cyan
W='\033[1;37m' # White
NC='\033[0m'   # No Color

# --- Typing Function ---
type_out() {
    text="$1"
    delay=${2:-0.03}
    for (( i=0; i<${#text}; i++ )); do
        echo -ne "${text:$i:1}"
        sleep $delay
    done
    echo ""
}

clear

# 1. Start with a "Glitch" Header
echo -e "${P}Initializing...${NC}"
sleep 0.4
echo -e "${DG}  _   _                      ____  ____  __  __ ${NC}"
echo -e "${DG} | \ | | ___ _   _ _ __ ___ |  _ \/ ___||  \/  |${NC}"
echo -e "${DG} |  \| |/ _ \ | | | '__/ _ \| | | \___ \| |\/| |${NC}"
echo -e "${DG} | |\  |  __/ |_| | | | (_) | |_| |___) | |  | |${NC}"
echo -e "${DG} |_| \_|\___|\__,_|_|  \___/|____/|____/|_|  |_|${NC}"
echo -e "${P} ---------------- [ CORE INTERFACE ] ---------------- ${NC}"

echo ""

# 2. Simulated System Check
echo -ne "${C}[ SIGNAL ] : ${NC}"; type_out "Searching for synaptic nodes..." 0.02
sleep 0.2
echo -ne "${C}[ MEMORY ] : ${NC}"; type_out "Allocating neural buffers... [OK]" 0.01
sleep 0.2
echo -ne "${C}[ UPLINK ] : ${NC}"; type_out "Synchronizing DSM protocols..." 0.04

echo ""

# 3. The "Pulse" Loading Bar
echo -ne "${W}SYNESTHESIA LOADING: ${NC}"
for i in {1..20}; do
    echo -ne "${DG}▰"
    sleep 0.05
done
echo -e "${G} 100%${NC}"

echo ""

# 4. Final Flashy Message
echo -e "${W}>> ${DG}NEURO DSM SERVICE DEPLOYED${NC} ${W}<<${NC}"
echo -e "${P}Status: ${G}COGNITIVE ENGINE ACTIVE${NC}"
echo -e "${C}Welcome back, Operator.${NC}"
echo ""

./main