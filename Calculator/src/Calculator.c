/*
    NOTES: - bug with number aproximation after any operation or equal is pressed, will be resolved through replacing everywhere where is 
                sprintf(a or b), %g -with-> %.14g 
           - need to add button pressed effect when numbers will be introduced with keyboard, not just with mouse
           - can add that if enter is pressed many times and after user introduces another number and then presses again enter it will replace "a" 
                from smalltextbox and will calculate with inputed value
*/
#include <string.h>

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "../headers/raygui.h"

#define NUM_LIMIT 15 //max number of digits in a number restricted by variable type

typedef enum {SMALL_TEXTBOX, BIG_TEXTBOX, ALL_TEXTBOX}       ClrTextbox;

// Controls Functions Declaration
//----------------------------------------------------------------------------------
static void Button0();
static void Button1();
static void Button2();
static void Button3();
static void Button4();
static void Button5();
static void Button6();
static void Button7();
static void Button8();
static void Button9();

static void SetOperation(const char*, char);
static void ButtonAdition();
static void ButtonSubstraction();
static void ButtonMultiplication();
static void ButtonDivision();

static void SetSpecialOperation(const char*, const char*, char);
static void ButtonSqrX();
static void ButtonSqrtX();
static void ButtonOneX();

static void ButtonPercent();
static void ButtonPlusMinus();
static void ButtonPoint();

static void ClearTextBoxes(ClrTextbox);
static void ButtonClear();
static void ButtonClearCell();
static void ButtonBackSpace();

static void PrintResult();
static void PrintSpecialResult();
static void PrintEnter(const char*, char);
static void SwitchEnterOperation(bool, char);
static void ButtonEnter();

static void DisplayNumber(char);

static const int screenWidth  = 376;
static const int screenHeight = 538;
//----------------------------------------------------------------------------------

// Define controls variables
//----------------------------------------------------------------------------------
char SmallTextBoxInputText[(NUM_LIMIT*2)+5] = {'\0'};   // TextBox: SMALL TextBoxInput//size destul pt a incapea 2 numere si operatiile
char BigTextBoxInputText[(NUM_LIMIT*2)+2+1] = {'\0'};   // TextBox: BIG TextBoxInput//size destul pt ca result sa incapa (19 caractere) %.14g
signed char BaseNumberCount                 = 0;        // Number of digits that form a number       

double a        = 0.0f;             //first number
double b        = 0.0f;             //sencod number
double result   = 0.0f;             //result of calculation
char operation  = 0;                //operation sign

bool EnterPressed       = false;    //flag if enter is pressed once
bool ActiveSecondNumber = false;    //flag if text in BigTextBoxInputText is for second of first number
bool ActivePoint        = false;    //flag if in BigTextBoxInputText is displayed point char
bool ErrorCase          = false;    //flag if a number is divided by 0, that will cause a coresponding output and behavier
//----------------------------------------------------------------------------------

//Define controls rectangles
//----------------------------------------------------------------------------------
Rectangle layoutRecs[27] =
{
    (Rectangle){ 0, 0, 376, 538 },      // 0 WindowBox: WindowBox000
    (Rectangle){ 10, 18, 358, 36 },     // 1 Small TextBox: TextBoxInput
    (Rectangle){ 10, 54, 358, 80 },     // 2 Big TextBox: TextBoxInput
    (Rectangle){ 190, 140, 86, 60 },    // 3 Button: ButtonClear
    (Rectangle){ 280, 204, 86, 60 },    // 4 Button: ButtonDivision
    (Rectangle){ 280, 268, 86, 60 },    // 5 Button: ButtonMultiplication
    (Rectangle){ 280, 332, 86, 60 },    // 6 Button: ButtonSubstraction
    (Rectangle){ 10, 268, 86, 60 },     // 7 Button: Button7
    (Rectangle){ 100, 268, 86, 60 },    // 8 Button: Button8
    (Rectangle){ 190, 268, 86, 60 },    // 9 Button: Button9
    (Rectangle){ 280, 396, 86, 60 },    // 10 Button: ButtonAdition
    (Rectangle){ 10, 332, 86, 60 },     // 11 Button: Button4
    (Rectangle){ 100, 332, 86, 60 },    // 12 Button: Button5
    (Rectangle){ 190, 332, 86, 60 },    // 13 Button: Button6
    (Rectangle){ 10, 396, 86, 60 },     // 14 Button: Button1
    (Rectangle){ 100, 396, 86, 60 },    // 15 Button: Button2
    (Rectangle){ 190, 396, 86, 60 },    // 16 Button: Button3
    (Rectangle){ 280, 460, 86, 60 },    // 17 Button: ButtonEnter
    (Rectangle){ 100, 460, 86, 60 },    // 18 Button: Button0
    (Rectangle){ 10, 140, 86, 60 },     // 19 Button: Percent
    (Rectangle){ 100, 140, 86, 60 },    // 20 Button: ClearCell
    (Rectangle){ 280, 140, 86, 60 },    // 21 Button: BackSpace
    (Rectangle){ 10, 204, 86, 60 },     // 22 Button: 1/x
    (Rectangle){ 100, 204, 86, 60 },    // 23 Button: x^2
    (Rectangle){ 190, 204, 86, 60 },    // 24 Button: x^(1/2)
    (Rectangle){ 10, 460, 86, 60 },     // 25 Button: PlusMinus
    (Rectangle){ 190, 460, 86, 60 }     // 26 Button: ButtonDelete
};
//------------------------------------------------------------------------------------

// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    InitWindow(screenWidth, screenHeight, "Calculator");    //initialize window
    GuiLoadStyle("../style/style_calc.rgs");                //load custom style
    SetTargetFPS(60);                                                                        

    BigTextBoxInputText[0] = '0';                           //when window is initialized first shown digit will be "0"

    while (!WindowShouldClose())                            // Detect window close button or ESC key
    {
        // Draw controls
        //----------------------------------------------------------------------------------
        BeginDrawing();

            GuiPanel(layoutRecs[0]);
            if (GuiTextBox(layoutRecs[1], SmallTextBoxInputText, NUM_LIMIT*2+5, false));
            if (GuiTextBox(layoutRecs[2], BigTextBoxInputText, NUM_LIMIT*2+5, false));
            if (GuiButton(layoutRecs[3], "C") ||((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) && IsKeyPressed(KEY_DELETE))) ButtonClear();
            if (GuiButton(layoutRecs[4], "÷") || (IsKeyPressed(KEY_SLASH) && !(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))) || IsKeyPressed(KEY_KP_DIVIDE)) ButtonDivision();
            if (GuiButton(layoutRecs[5], "x") || ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) && IsKeyPressed(KEY_EIGHT)) || IsKeyPressed(KEY_KP_MULTIPLY)) ButtonMultiplication();
            if (GuiButton(layoutRecs[6], "-") || (IsKeyPressed(KEY_MINUS) && !(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))) || IsKeyPressed(KEY_KP_SUBTRACT)) ButtonSubstraction();
            if (GuiButton(layoutRecs[7], "7") || (IsKeyPressed(KEY_SEVEN) && !(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))) || IsKeyPressed(KEY_KP_7)) Button7();
            if (GuiButton(layoutRecs[8], "8") || ((IsKeyPressed(KEY_EIGHT) || IsKeyPressed(KEY_KP_8)) && !(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)))) Button8();
            if (GuiButton(layoutRecs[9], "9") || (IsKeyPressed(KEY_NINE) && !(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))) || IsKeyPressed(KEY_KP_9)) Button9();
            if (GuiButton(layoutRecs[10], "+") || ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) && IsKeyPressed(KEY_EQUAL)) || IsKeyPressed(KEY_KP_ADD)) ButtonAdition();
            if (GuiButton(layoutRecs[11], "4") || (IsKeyPressed(KEY_FOUR) && !(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))) || IsKeyPressed(KEY_KP_4)) Button4();
            if (GuiButton(layoutRecs[12], "5") || ((IsKeyPressed(KEY_FIVE) || IsKeyPressed(KEY_KP_5)) && !(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)))) Button5();
            if (GuiButton(layoutRecs[13], "6") || (IsKeyPressed(KEY_SIX) && !(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))) || IsKeyPressed(KEY_KP_6)) Button6();
            if (GuiButton(layoutRecs[14], "1") || (IsKeyPressed(KEY_ONE) && !(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))) || IsKeyPressed(KEY_KP_1)) Button1();
            if (GuiButton(layoutRecs[15], "2") || (IsKeyPressed(KEY_TWO) && !(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))) || IsKeyPressed(KEY_KP_2)) Button2();
            if (GuiButton(layoutRecs[16], "3") || (IsKeyPressed(KEY_THREE) && !(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))) || IsKeyPressed(KEY_KP_3)) Button3();
            if (GuiButton(layoutRecs[17], "=") || IsKeyPressed(KEY_KP_ENTER) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_EQUAL) || (IsKeyPressed(KEY_EQUAL) && !(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)))) ButtonEnter();
            if (GuiButton(layoutRecs[18], "0") || (IsKeyPressed(KEY_ZERO) && !(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))) || IsKeyPressed(KEY_KP_0)) Button0();
            if (GuiButton(layoutRecs[19], "%") || ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) && IsKeyPressed(KEY_FIVE))) ButtonPercent();
            if (GuiButton(layoutRecs[20], "CE") || ((IsKeyPressed(KEY_DELETE) && !(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))))) ButtonClearCell();
            if (GuiButton(layoutRecs[21], "<-") || (IsKeyPressed(KEY_BACKSPACE) && !(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)))) ButtonBackSpace();
            if (GuiButton(layoutRecs[22], "1/x")) ButtonOneX();
            if (GuiButton(layoutRecs[23], "x²")) ButtonSqrX();
            if (GuiButton(layoutRecs[24], "x^(1/2)") || (IsKeyPressed(KEY_TWO) && (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)))) ButtonSqrtX();
            if (GuiButton(layoutRecs[25], "+/-")) ButtonPlusMinus();
            if (GuiButton(layoutRecs[26], ".") || IsKeyPressed(KEY_PERIOD) || IsKeyPressed(KEY_KP_DECIMAL)) ButtonPoint();
            
            GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_RIGHT);//aligne text of textboxes to right

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    CloseWindow();    //De-Initialization: Close window and OpenGL context

    return 0;
}

//Button functions
//----------------------------------------------------------------------------------
static void Button0() 
{
    if(BigTextBoxInputText[0] != '0' || BigTextBoxInputText[1] == '.') //BigTextBoxInputText[1] == '.' --> when i put 0., so I can put another 0 after point
        DisplayNumber('0');
}
static void Button1() { DisplayNumber('1'); }
static void Button2() { DisplayNumber('2'); }
static void Button3() { DisplayNumber('3'); }
static void Button4() { DisplayNumber('4'); }
static void Button5() { DisplayNumber('5'); }
static void Button6() { DisplayNumber('6'); }
static void Button7() { DisplayNumber('7'); }
static void Button8() { DisplayNumber('8'); }
static void Button9() { DisplayNumber('9'); }
//----------------------------------------------------------------------------------

//String modification functions
//----------------------------------------------------------------------------------
static void DeactivateVariables()// Deactivate all important variables
{
    a = b = result = operation = BaseNumberCount = 0;
    EnterPressed = ActiveSecondNumber = ActivePoint = ErrorCase = false;
}

static void ClearTextBoxes(ClrTextbox textbox)// Clear textboxes in dependence what argument it have for each textbox
{
    if(textbox == SMALL_TEXTBOX)
        memset(SmallTextBoxInputText, 0, sizeof(SmallTextBoxInputText));
    
    else if(textbox == BIG_TEXTBOX)
        memset(BigTextBoxInputText, 0, sizeof(BigTextBoxInputText));
    
    else if(textbox == ALL_TEXTBOX)
    {
        memset(SmallTextBoxInputText, 0, sizeof(SmallTextBoxInputText));
        memset(BigTextBoxInputText, 0, sizeof(BigTextBoxInputText));
    }
}

static void ErrorDivideByZero()// Exception function
{
    strcpy(BigTextBoxInputText, "Cannot divide by zero");//print error message
    DeactivateVariables();                               //deactivate variables    
    operation = '/';                                     //set current operation to '/' if enter will be pressed again, so it will show 0, instead of copying the content of bigtextbox to smalltextbox    
    ErrorCase = true;                                    //set error flag as true    
}

static void ButtonClear()// Clear all content from textboxes
{
    ClearTextBoxes(ALL_TEXTBOX); //clear all textboxes
    DeactivateVariables();       //deactivate all variables
    BigTextBoxInputText[0] = '0';//put first char in bigtextbox to be 0
}

static void ButtonClearCell()// Clear only bigtextbox, or clear all textboxes if the enter was pressed 
{
    if(!EnterPressed)                 //big text box will be cleared if enter is not pressed 
    {
        ClearTextBoxes(BIG_TEXTBOX); //clear bigtextbox
        BigTextBoxInputText[0] = '0';//assigne 0 to bigtextbox
        BaseNumberCount=0;           //will be formed new number then    
    }
    else ButtonClear();              //if the enter already is pressed it will clear all textboxes    
}

static void ButtonBackSpace()// Delete one digit one by one
{
    if(!EnterPressed)                                                       //if enter is not pressed 
        if(BaseNumberCount)                                                 //bigtextbox has at least one digit
        {
            if(BigTextBoxInputText[BaseNumberCount-1] == '.')               //if the char that will be deleted is point then
            {
                ActivePoint = false;                                        //flat that says that point is active, will be false
                
                if(BigTextBoxInputText[BaseNumberCount-2] == '0')           //if the precedent char to point is 0 then delete and this char to prevent appearance of bug when input is 0123 after deleting point in "0."
                    BigTextBoxInputText[--BaseNumberCount] = '\0';          //if the precendent char to point is 0 then it will be also deleted --> here the point will be deleted 
            }
            BigTextBoxInputText[--BaseNumberCount] = '\0';                  // --> here the 0 will be deleted, and deleted any char
            
            if(BigTextBoxInputText[0] == '\0') BigTextBoxInputText[0] = '0';//if the 0 index char in bigtextbox will be null char, will be assigned 0 to it --> here will be autocompleted with 0 after deleting any last char    
        }
}
//----------------------------------------------------------------------------------

//Functions for operations
//----------------------------------------------------------------------------------
static void SetOperation(const char *string_operation, char setted_operation)// Set a simple operations
{
    ActiveSecondNumber = true;                                          //after a simple operation is pressed flag for second number will be activated
    ActivePoint = false;                                                //flag for point char will be deactivated to be introduced to new number
    //EnterPressed = false;                                               
    
    if(BigTextBoxInputText[BaseNumberCount-1] == '.') ButtonBackSpace();//if point char will be introduces to string number in bigtextbox and then will be pressed simple operation then the point will be deleted in smalltextbox
    
    sscanf(BigTextBoxInputText, "%lg", &a);                             //if a simple operation will be pressed then a numbe from bigtextbox will be scaned to "a" variable
    /**/sprintf(SmallTextBoxInputText, "%g", a);                        //"a" will be printed with aproximation in smalltextbox (0.00000000 --> 0)
    //strcpy(SmallTextBoxInputText, BigTextBoxInputText);               //if it will be decommented and commented marked lines then 0.00000000 introduced in bigtextbox in smalltextbox will look the same, just for first enter
    strcat(SmallTextBoxInputText, string_operation);                    //append to smalltextbox the operation string
    
    BaseNumberCount = 0;                                                //number counter will be ssigned to 0, to introduce another number
    operation = setted_operation;                                       //to global variable "operation" will be assigned argument char of function
    b=a;                                                                //"b" variable will be the same as scanned "a" if the enter will be pressed without introduction of "b" so it will be the same variable as "a"
    
    /**/sprintf(BigTextBoxInputText, "%g", b);                          //bigtextbox will print "a" assigned to "b", so if the marked lines will be kept then it will print aproximated numbers, else it will print exactly indroduced numbers for first enter
}

static void ButtonAdition()       { SetOperation(" +", '+'); }
static void ButtonSubstraction()  { SetOperation(" -", '-'); }
static void ButtonMultiplication(){ SetOperation(" x", 'x'); }
static void ButtonDivision()      { SetOperation(" /", '/'); }

static void PrintSpecialResult()// Print result of special operations
{
    strcat(SmallTextBoxInputText, " =");            //else append to smalltextbox equal sign
    sprintf(BigTextBoxInputText, "%.14g", result);  //update the bigtextbox with result displayed with 14 digits after point                                       
}

static void SetSpecialOperation(const char *first_part_string, const char *second_part_string, char char_operation)// Set special operations
{
    //ActiveSecondNumber = false;
    ActivePoint = false;                                                   //Deactivate flag of point, so if "a" was with point then when point will be pressed it will print 0.
    
    if(BigTextBoxInputText[BaseNumberCount-1] == '.') ButtonBackSpace();   //if number have point and next char is nothing then this point will be deleted to be printed in smalltextbox and bigtextbox
    sscanf(BigTextBoxInputText, "%lg", &a);                                //scan "a" variable from bigtextbox    
    strcpy(SmallTextBoxInputText, first_part_string);                      //copy in smalltextbox first part of necessary string of operation
    
    if(ErrorCase) strcat(SmallTextBoxInputText, "0");                      //if is error case then just append to smalltextbox number 0
    else strcat(SmallTextBoxInputText, BigTextBoxInputText);               //else append to smalltextbox content(number) of bigtextbox
    
    strcat(SmallTextBoxInputText, second_part_string);                     //append to smalltextbox second part of string argument of function
    BaseNumberCount = 0;                                                   //cancellation of number counter to be introduced new number
    
    switch(char_operation)                                                 //switch operation, do necessary calculation and print result    
    {
        case '*': 
                result = a*a; 
                PrintSpecialResult();  
                break;
        
        case '^': 
                result = sqrt(a);
                PrintSpecialResult();
                break;
       
        case '\\':
                result = 1/a;
                if(a==0) ErrorDivideByZero();                              //if in special division operation, divided operand is 0 then it's invalid operation and the Error function is called
                else PrintSpecialResult();                                 //else just print result as a normal special operation
                break;
    }
    
    a = result;                                                            //"a" will be assigned with "result" to be a new variable after the special operation will be pressed more than once
    EnterPressed = true;                                                   //Enter pressed flag will be assigned with true, so before new numbers that will be introduced, it will clear all textboxes
}

static void ButtonSqrX()  { SetSpecialOperation("sqr(", ")", '*'); }
static void ButtonSqrtX() { SetSpecialOperation("sqrt(", ")", '^'); }
static void ButtonOneX()  { SetSpecialOperation("1 / (", ")", '\\'); }
//----------------------------------------------------------------------------------

// Operation functions that modifies strings
//----------------------------------------------------------------------------------
static void ButtonPercent()// Calculate percent value of variale "a"
{
    if(a)                                                                       //if variable "a" is introduced then the percent button can be applied
    {
        sscanf(BigTextBoxInputText, "%lg", &b);                                 //scan variable "b" from bigtextbox
        b = (b*a)/100;                                                          //transform it to percent of variable "a"
        sprintf(BigTextBoxInputText, "%g", b);                                  //update bigtextbox with value of variable "b"
        
        if(EnterPressed) sprintf(SmallTextBoxInputText, BigTextBoxInputText);   //if enter is pressed, and is pressed percent button (in that case a == b) it will print in small and big textboxes the result of percent button (variable "b")
    }
}

static void ButtonPlusMinus()// Asigne minus to positive number and vice versa
{
    char neg_numer_buffer[NUM_LIMIT+5] = {'\0'};                //buffer for negative number
    char *ptr;                                                  //pointer that will point to the string part in bigtextbox in function strtod
    if(strtod(BigTextBoxInputText, &ptr) != 0)                  //if the double value of string is different with 0, so to 0.00001 it will assigne a minus sign 
    {
        if(BigTextBoxInputText[0] != '-')                       //if first byte of string is not - (is positive)
        {
            neg_numer_buffer[0] = '-';                          //assigne to first byte of empty negative buffer, minus sign 
            strcat(neg_numer_buffer, BigTextBoxInputText);      //append to the rest of negative buffer the essential number that has been written in bigtextbox
            strcpy(BigTextBoxInputText, neg_numer_buffer);      //then assigne to bigtextbox - negative buffer to be displayed in gui
            BaseNumberCount++;                                  //increase number counter for negative sign, to avoid problems when insert or delete a char
        }
        
        else                                                    //if number is already with negative sign            
        {
            strcpy(neg_numer_buffer, BigTextBoxInputText);      //copy it to negative buffer to be changed in continue
            for(int i = 0 ; neg_numer_buffer[i]; i++)           //while it won't be the finish of negative buffer
                BigTextBoxInputText[i] = neg_numer_buffer[i+1]; //assigne to bigtextbox just numbers from copied negative buffer, avoiding negative sign
            BaseNumberCount--;                                  //decrease number counter to delete count of negative sign
        }
    }
}

static void ButtonPoint()// Add point to number transforming it to double 
{
    if(!ActivePoint)        //if in string is not already present any point then    
        DisplayNumber('.'); //display point to string in bigtextbox

    ActivePoint = true;     //activate point flag, so point is present in string
}
//----------------------------------------------------------------------------------

//Enter and result print funcitons
//----------------------------------------------------------------------------------
static void PrintResult()// Print result of simple operations
{
    char second_number_buffer[NUM_LIMIT] = {'\0'};      //create an empty buffer for second number
    sprintf(second_number_buffer, "%g", b);             //assigne to second number buffer the value of "b" (as string) // also(but in some cases it is importnat with more than one enter pressed) to aproximate bigtextbox numbers introduced by users (0.00000 --> 0)

    strcat(SmallTextBoxInputText, second_number_buffer);//append to smalltextbox (aproximated) second number buffer
    strcat(SmallTextBoxInputText, " =");                //addig equal sign
    sprintf(BigTextBoxInputText, "%.14g", result);      //update bigtextbox with result of operation    
    a=result;                                           //assigne to "a" value of "result", to be able to press enter more time and to update the calculations
}

static void SwitchEnterOperation(bool first_enter, char char_operation)// General switch for enter, in case of multiple enter pressing and one enter pressing
{
    switch(char_operation)                         //switch operation to calculate result and to print it if it is first enter, else it will just calculate the result and update pe small and big textboxes in ButtonEnter function
    {
        case '+':
                result = a+b;
                if(first_enter) PrintResult();
                break;
        
        case '-':   
                result = a-b;
                if(first_enter) PrintResult();
                break;
        
        case 'x':
                result = a*b;
                if(first_enter) PrintResult();
                break;
        
        case '/':
                result = a/b;
                if(first_enter)                  //if it is first enter pressed    
                {
                    if(b==0) ErrorDivideByZero();//if the number that is divided is 0, then it will print error message
                    else PrintResult();          //else it will just print a normal result
                } break;
    }  
}

static void PrintEnter(const char *string_operation, char char_operation)// Print result of calculations in dependece how many time enter is pressed
{
    if(BigTextBoxInputText[BaseNumberCount-1] == '.') ButtonBackSpace();            //if last char of string is point what is not leaded with something else, then it will be deleted

    if(!EnterPressed)                                                               //if enter is not already pressed (if it is pressed once) 
    {
        sscanf(BigTextBoxInputText, "%lg", &b); strcat(SmallTextBoxInputText, " "); //scan "b" variable from bigtextbox and append space to smalltextbox for space between operation sign and next number
        SwitchEnterOperation(true, char_operation);                                 //switch char_operation argument, and set first_enter argument in SwitchEnterOperation function as true, so it is first enter pressed
    }
    
    else                                                                            //if enter is second or more then
    {
        SwitchEnterOperation(false, char_operation);                                //set first_enter argument as false, that means that it is not first enter
        
        if(!ErrorCase)                                                              //if it's not error case then 
        {
            sprintf(SmallTextBoxInputText, "%g", a);                                //replace(update) in smalltextbox first number (variable "a")   
            strcat(SmallTextBoxInputText, string_operation);                        //append againg string operation to smalltextbox
            PrintResult();                                                          //print normal enter result after was called SwitchEnterOperation
        }
        
        else                                                                        //if error case is active then after enter is pressed second time
        {
            ClearTextBoxes(ALL_TEXTBOX);                                            //all textboxes will be cleared                
            DeactivateVariables();                                                  //deactevated all variables even ErrorCase  
            BigTextBoxInputText[0] = '0';                                           //0 will be putted in bigtextbox as first char(digit)
        }
    }
}

static void ButtonEnter()// Print result in dependence just what operations was pressed 
{
    switch(operation)                                                               //Print result of calculation of certain operation    
    {
        case '+': PrintEnter(" + ", '+'); break;
        case '-': PrintEnter(" - ", '-'); break;
        case 'x': PrintEnter(" x ", 'x'); break;
        case '/': PrintEnter(" / ", '/'); break;
                
        default:                                                                    //if no operation was introduced then     
                if(BigTextBoxInputText[BaseNumberCount-1] == '.') ButtonBackSpace();//also if last char of string in bigtextbox was point followed by nothing then it will be deleted
                strcpy(SmallTextBoxInputText, BigTextBoxInputText);                 //will be assigned to smalltextbox content of bigtextbox  
                strcat(SmallTextBoxInputText, " =");                                //and then will be just added the equal sign
    }

    EnterPressed = true;                                                            //enter flag will be setted as pressed, so it will mean that once it was pressed and will influence PrintEnter() function that will display right content to textboxes
    ActiveSecondNumber = false;                                                     //second number flag will be deactivated so next introduced number will be setted as first number// if deactivate this, after introducing new number it will append inputed number to smalltextbox and calculate the sum of three numbers
}
//----------------------------------------------------------------------------------

static void DisplayNumber(char ch)// Display char on bigtextbox
{
    if(EnterPressed && !ActiveSecondNumber)                 //if enter is pressed and second number flag is deactivated after enter then
    {
        ClearTextBoxes(ALL_TEXTBOX);                        //clear all text boxes, deactivate variables and set enter flag as false to introduce then another number
        DeactivateVariables();
    }
  
    if(ActiveSecondNumber)                                  //if second number flag is active (it's phase to introduce second number)
    {
        ClearTextBoxes(BIG_TEXTBOX);                        //clear just bigtextbox
        ActiveSecondNumber = false;                         //set this flag as false so at next digit introduced it will not clear again bigtextbox
    }
    
    if(EnterPressed) EnterPressed = false;                  //if enter was pressed, then after introducing a new digit it will be setted as not pressed
    
    //if(!EnterPressed)                                       //only when enter is not pressed you can introduce new numbers
    //{
        if(BaseNumberCount < NUM_LIMIT)                     //if number counter is less than setted limit it will read the character
            BigTextBoxInputText[BaseNumberCount++] = ch;
        
        if(BigTextBoxInputText[0] == '.')                   //if first character introduced is point then it will set first char as "0" and then    
        {
            BigTextBoxInputText[0] = '0';
            BigTextBoxInputText[BaseNumberCount++] = ch;    //next char will be that point
        }
    //}
}