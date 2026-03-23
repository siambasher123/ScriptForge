#!/bin/bash

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
WHITE='\033[1;37m'
NC='\033[0m' # No Color

# Clear screen
clear

# Main loop
while true; do
    # Display header
    echo -e "${BLUE}╔══════════════════════════════════════════════════════════╗${NC}"
    echo -e "${BLUE}║${WHITE}              GOBLIN AI COMPILER TERMINAL v1.0             ${BLUE}║${NC}"
    echo -e "${BLUE}║${CYAN}           Transform DSL to Unity C# Code                  ${BLUE}║${NC}"
    echo -e "${BLUE}╚══════════════════════════════════════════════════════════╝${NC}"
    echo ""
    echo -e "${YELLOW}Current Directory:${NC} $(pwd)"
    echo ""
    echo -e "${GREEN}[1]${NC} Compile input.txt"
    echo -e "${GREEN}[2]${NC} Compile custom file"
    echo -e "${GREEN}[3]${NC} View last output (full)"
    echo -e "${GREEN}[4]${NC} Clean build files"
    echo -e "${GREEN}[5]${NC} Show statistics"
    echo -e "${GREEN}[6]${NC} Build compiler"
    echo -e "${GREEN}[7]${NC} Exit"
    echo ""
    echo -ne "${CYAN}Select option (1-7): ${NC}"
    read choice

    case $choice in
        1)
            clear
            echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}"
            echo -e "${GREEN}Compiling input.txt...${NC}"
            echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}\n"
            
            # Check if compiler exists
            if [ ! -f "./compiler" ]; then
                echo -e "${YELLOW}Compiler not found. Building first...${NC}"
                make clean > /dev/null 2>&1
                make > /dev/null 2>&1
                if [ $? -ne 0 ]; then
                    echo -e "${RED}Failed to build compiler!${NC}"
                    read -p "Press Enter to continue..."
                    continue
                fi
            fi
            
            # Check if input.txt exists
            if [ ! -f "input.txt" ]; then
                echo -e "${RED}Error: input.txt not found!${NC}"
                read -p "Press Enter to continue..."
                continue
            fi
            
            # Run compiler
            echo -e "${YELLOW}Starting compilation...${NC}\n"
            ./compiler input.txt
            
            if [ $? -eq 0 ] && [ -f "output.cs" ]; then
                echo -e "\n${GREEN}✓ Compilation successful!${NC}"
                echo -e "${CYAN}Output saved to: output.cs${NC}"
                
                # Ask if user wants to view the output
                echo ""
                echo -ne "${CYAN}View the generated code? (y/n): ${NC}"
                read view_choice
                if [ "$view_choice" = "y" ] || [ "$view_choice" = "Y" ]; then
                    clear
                    echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}"
                    echo -e "${GREEN}Generated C# Code (Full)${NC}"
                    echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}\n"
                    cat output.cs
                    echo -e "\n${BLUE}══════════════════════════════════════════════════════════${NC}"
                fi
            else
                echo -e "\n${RED}✗ Compilation failed!${NC}"
            fi
            read -p "Press Enter to continue..."
            ;;
            
        2)
            clear
            echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}"
            echo -e "${GREEN}Compile Custom File${NC}"
            echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}\n"
            
            ls -la *.txt 2>/dev/null
            if [ $? -ne 0 ]; then
                echo -e "${YELLOW}No .txt files found in current directory.${NC}"
            fi
            
            echo ""
            echo -ne "${CYAN}Enter filename: ${NC}"
            read filename
            
            if [ ! -f "$filename" ]; then
                echo -e "${RED}Error: File '$filename' not found!${NC}"
                read -p "Press Enter to continue..."
                continue
            fi
            
            echo -e "\n${YELLOW}Compiling $filename...${NC}\n"
            ./compiler "$filename"
            
            if [ $? -eq 0 ] && [ -f "output.cs" ]; then
                echo -e "\n${GREEN}✓ Compilation successful!${NC}"
                
                # Ask if user wants to view the output
                echo ""
                echo -ne "${CYAN}View the generated code? (y/n): ${NC}"
                read view_choice
                if [ "$view_choice" = "y" ] || [ "$view_choice" = "Y" ]; then
                    clear
                    echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}"
                    echo -e "${GREEN}Generated C# Code (Full)${NC}"
                    echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}\n"
                    cat output.cs
                    echo -e "\n${BLUE}══════════════════════════════════════════════════════════${NC}"
                fi
            else
                echo -e "\n${RED}✗ Compilation failed!${NC}"
            fi
            read -p "Press Enter to continue..."
            ;;
            
        3)
            clear
            echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}"
            echo -e "${GREEN}Generated C# Code (Full)${NC}"
            echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}\n"
            
            if [ -f "output.cs" ]; then
                echo -e "${CYAN}File: output.cs ($(wc -l < output.cs) lines, $(wc -c < output.cs) bytes)${NC}\n"
                echo -e "${PURPLE}════════════ START OF FILE ════════════${NC}"
                cat output.cs
                echo -e "\n${PURPLE}═════════════ END OF FILE ═════════════${NC}"
            else
                echo -e "${RED}No output.cs file found!${NC}"
            fi
            read -p "Press Enter to continue..."
            ;;
            
        4)
            clear
            echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}"
            echo -e "${GREEN}Cleaning Build Files${NC}"
            echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}\n"
            
            make clean
            echo -e "\n${GREEN}✓ Clean complete!${NC}"
            read -p "Press Enter to continue..."
            ;;
            
        5)
            clear
            echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}"
            echo -e "${GREEN}Compiler Statistics${NC}"
            echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}\n"
            
            if [ -f "input.txt" ]; then
                LINES=$(wc -l < input.txt)
                VARS=$(grep -c "^[a-z]* " input.txt)
                MODES=$(grep -c "mode " input.txt)
                TRANS=$(grep -c -E "when|unless|ifnot" input.txt)
                LOOPS=$(grep -c -E "orbit|pulse|bounce" input.txt)
                
                echo -e "${CYAN}Input File:${NC} input.txt"
                echo -e "${CYAN}Lines:${NC} $LINES"
                echo -e "${CYAN}Variables:${NC} $VARS"
                echo -e "${CYAN}Modes:${NC} $MODES"
                echo -e "${CYAN}Transitions:${NC} $TRANS"
                echo -e "${CYAN}Loops:${NC} $LOOPS"
            else
                echo -e "${RED}No input.txt found${NC}"
            fi
            
            echo ""
            if [ -f "output.cs" ]; then
                OUT_LINES=$(wc -l < output.cs)
                OUT_SIZE=$(wc -c < output.cs)
                echo -e "${CYAN}Output File:${NC} output.cs"
                echo -e "${CYAN}Lines:${NC} $OUT_LINES"
                echo -e "${CYAN}Size:${NC} $OUT_SIZE bytes"
            fi
            
            echo ""
            if [ -f "compiler" ]; then
                COMP_SIZE=$(wc -c < compiler)
                echo -e "${CYAN}Compiler:${NC} $(date -r compiler)"
                echo -e "${CYAN}Size:${NC} $COMP_SIZE bytes"
            fi
            read -p "Press Enter to continue..."
            ;;
            
        6)
            clear
            echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}"
            echo -e "${GREEN}Building Compiler${NC}"
            echo -e "${BLUE}══════════════════════════════════════════════════════════${NC}\n"
            
            make clean > /dev/null 2>&1
            make
            
            if [ $? -eq 0 ] && [ -f "compiler" ]; then
                echo -e "\n${GREEN}✓ Compiler built successfully!${NC}"
            else
                echo -e "\n${RED}✗ Build failed!${NC}"
            fi
            read -p "Press Enter to continue..."
            ;;
            
        7)
            clear
            echo -e "${GREEN}Thank you for using GOBLIN AI COMPILER!${NC}"
            echo -e "${CYAN}Goodbye!${NC}"
            sleep 1
            clear
            exit 0
            ;;
            
        *)
            echo -e "${RED}Invalid option!${NC}"
            sleep 1
            ;;
    esac
    
    clear
done



