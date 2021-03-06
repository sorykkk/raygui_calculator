/*NOTES:

    -dupa ce fac = doar cu numar a, apoi introduc alt numar, trebuie sa se curate bigtextbox, dar smalltextbox sa fie la fel
    -adaugarea temporary memory history
    -trebuie sa lucrez cu stringurile mai mult decat cu cifrele --> de exemplu la stergerea numarul sa fac string[end] = '\0'; pana cand nu este apasat enter sau o operatie
    -trebuie de lacrat cu double de la inceput 
    -de scris totul de la inceput si mai eficient
*/
#include <string.h>

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "../headers/raygui.h"

#define NUM_LIMIT 9 //maximum number of digits in a number restricted by variable type

// Controls Functions Declaration
//----------------------------------------------------------------------------------
typedef enum {COMPLETE_ENTER, INCOMPLETE_ENTER, PERCENT_ENTER} EnterPhase;
typedef enum {NO_EXCEPTION, EXCEPTION}                         SpecialException;
typedef enum {SMALL_TEXT_BOX, BIG_TEXT_BOX, ALL_TEXT_BOXES}    TextBoxes;

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

static void ButtonClear();
static void ButtonClearCell();
static void ButtonBackSpace();
static void ClearTextBoxes(TextBoxes);
static void DeactivateTextBoxVariables();
static void ErrorDivideByZero();

static void SetOperation(const char*, char);
static void ButtonAdition();
static void ButtonSubstraction();
static void ButtonMultiplication();
static void ButtonDivision();

static void SpecialOperationOutput(const char*);
static void EnterSpecialOperation(SpecialException, const char*);
static void SetSpecialOperation(const char*, const char*, char);
static void ButtonSqrX();
static void ButtonSqrtX();
static void ButtonOneX();
static void ButtonPercent();
static void Negation(long long*);
static void ButtonPlusMinus();
//static void ButtonPoint();

static void PrintResultDeactivateVar();
static void PrintEnterResult(EnterPhase);
static void EnterPercentSignCase(char);
static void ButtonEnter();
static void FormNumber(long long*,char);
static void DisplayNumber(char);

static int screenWidth  = 376;
static int screenHeight = 538;
//----------------------------------------------------------------------------------

// Calculator: controls initialization
// Define controls variables
//----------------------------------------------------------------------------------
char SmallTextBoxInputText[(NUM_LIMIT*2)+10] = {'\0'};       // TextBox: SMALL TextBoxInput//size destul pt a incapea 2 numere si operatiile
char BigTextBoxInputText[(NUM_LIMIT*2)+2+1]  = {'\0'};       // TextBox: BIG TextBoxInput//size destul pt ca result sa incapa (19 caractere) %.14g
signed char BaseNumberCount                   = 0;           // Number of digits that form a number       

long long int a = 0;        //first number
long long int b = 0;        //sencod number
double result   = 0.0f;     //result of calculation
double proc     = 0.0f;     //percent result
char operation  = 0;        //operation sign

bool ActiveSecondNumber   = false; //flag daca se poate de introdus al doilea numar
bool EnterAlreadyPressed  = false; //flag if enter is already pressed
bool ErrorCase            = false; //flag if the number is divided by 0
bool ActivePercentSign    = false; //flag if the special operation percent is pressed
//----------------------------------------------------------------------------------

//Define controls rectangles
//------------------------------------------------------------------------------------
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
    (Rectangle){ 190, 460, 176, 60 },   // 17 Button: ButtonEnter
    (Rectangle){ 100, 460, 86, 60 },    // 18 Button: Button0
    (Rectangle){ 10, 140, 86, 60 },     // 19 Button: Percent
    (Rectangle){ 100, 140, 86, 60 },    // 20 Button: ClearCell
    (Rectangle){ 280, 140, 86, 60 },    // 21 Button: BackSpace
    (Rectangle){ 10, 204, 86, 60 },     // 22 Button: 1/x
    (Rectangle){ 100, 204, 86, 60 },    // 23 Button: x^2
    (Rectangle){ 190, 204, 86, 60 },    // 24 Button: x^(1/2)
    (Rectangle){ 10, 460, 86, 60 }      // 25 Button: PlusMinus
    //(Rectangle){ 190, 460, 86, 60 },  // 19 Button: ButtonDelete
};
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    InitWindow(screenWidth, screenHeight, "Calculator"); //initialize window
    GuiLoadStyle("../style/style_calc.rgs");            //load custom style
    SetTargetFPS(60);                                       

    BigTextBoxInputText[0] = '0';                        //when window is initialized first shown digit

    while (!WindowShouldClose())                         // Detect window close button or ESC key
    {
        // Draw controls
        //----------------------------------------------------------------------------------
        BeginDrawing();
                                                        //trebuie sa adaug ca la apasarea pe keyboard sa se simuleze apasarea cu mouse
            GuiPanel(layoutRecs[0]);
            if (GuiTextBox(layoutRecs[1], SmallTextBoxInputText, 128, false));
            if (GuiTextBox(layoutRecs[2], BigTextBoxInputText, 128, false));
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
            //if (GuiButton(layoutRecs[19], ".") || IsKeyPressed(KEY_PERIOD) || IsKeyPressed(KEY_KP_DECIMAL)) ButtonPoint();

            GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_RIGHT);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//Numeral buttons definition
//------------------------------------------------------------------------------------------
static void Button0()
{
    if(BigTextBoxInputText[0] != '0' || !result)//!result este pt afisarea 0 cand este apasat 0, in timp ce rezultatul deja contine 0 (corecteaza buggul)//cand se apasa 0 nu conteaza in ce conditie, smalltextbox se va curati, iar daca bigtextbox >0 se va curati si el
        DisplayNumber('0');

    EnterAlreadyPressed = false;//se face by default anularea la enter pt introducerea noilor cifre
}
static void Button1() { DisplayNumber('1'); EnterAlreadyPressed = false; }
static void Button2() { DisplayNumber('2'); EnterAlreadyPressed = false; }
static void Button3() { DisplayNumber('3'); EnterAlreadyPressed = false; }
static void Button4() { DisplayNumber('4'); EnterAlreadyPressed = false; }
static void Button5() { DisplayNumber('5'); EnterAlreadyPressed = false; }
static void Button6() { DisplayNumber('6'); EnterAlreadyPressed = false; }
static void Button7() { DisplayNumber('7'); EnterAlreadyPressed = false; }
static void Button8() { DisplayNumber('8'); EnterAlreadyPressed = false; }
static void Button9() { DisplayNumber('9'); EnterAlreadyPressed = false; }
//-------------------------------------------------------------------------------------------

//String manipulating functions
//-------------------------------------------------------------------------------------------
static void ButtonClear()                               //clear text boxes by deactivating all variables and putting 0 as first digit
{
    DeactivateTextBoxVariables();
    ClearTextBoxes(ALL_TEXT_BOXES);
    BigTextBoxInputText[0] = '0';
}

static void ButtonClearCell()                           //clear big text box 
{
    if(!EnterAlreadyPressed || operation != 0)          //big text box will be cleared if enter is not pressed or an operation sign is pressed
    {
        ClearTextBoxes(BIG_TEXT_BOX);
        if(!ActiveSecondNumber) a =0;                   //if the second number is not activated then it's only a that is introducing and it will be assigned with 0
        if(ActiveSecondNumber) b =0;                    //if the second number is activated then it will be assigned with 0

        BigTextBoxInputText[0] = '0';
        BaseNumberCount=0;
    }
    else ButtonClear();                                 //if enter is pressed and is used an operation then it will clear all text boxes    
}

static void ButtonBackSpace()
{
    if(!EnterAlreadyPressed)
    {
        if(BaseNumberCount > 0)
        {
            BigTextBoxInputText[--BaseNumberCount] = '\0';//last digit will be assigned to null

            if(!ActiveSecondNumber) a /= 10;              //if second number is not activated then will be decreased 
            if(ActiveSecondNumber)  b /= 10;              //is second number is active then it will clear it one by one    
        }

        if(BigTextBoxInputText[0] == '\0')                //if first character in big text box buffer is null then it will be assigned with 0
            BigTextBoxInputText[0] = '0';

        if(BaseNumberCount == 1 && BigTextBoxInputText[0] == '-') //when the number i.e. will be -1 and the backspace will be pressed it will remove 1 (and cuz "-" si counted in base number count) that 1 BaseNumberCount will be that negative sign, and if it is exactly "-" char then it will be replaced with "0" and decreases the surplus BaseNumberCount
        {
            BigTextBoxInputText[0] = '0';                         //baseNumberCount is >0 and when the backspace will be pressed it will be removed and the negative signe also will be removed automatically
            BaseNumberCount--;
        }
    }

    else
    {
        if(!ErrorCase)                                           //if enter is pressed and if is not the case when a number is divided by 0 and is pressed backspace then small box will be cleared  
            ClearTextBoxes(SMALL_TEXT_BOX);

        if(ErrorCase)                                            //else is enter and error case then all text boxes will be cleared
        {
            ClearTextBoxes(ALL_TEXT_BOXES);
            BigTextBoxInputText[0] = '0';
        }
    }
}

static void ClearTextBoxes(TextBoxes text_box)                  //clear text boxes by cases
{
    if(text_box == SMALL_TEXT_BOX) memset(SmallTextBoxInputText,0,sizeof(SmallTextBoxInputText));
    if(text_box == BIG_TEXT_BOX) memset(BigTextBoxInputText,0,sizeof(BigTextBoxInputText));
    if(text_box == ALL_TEXT_BOXES)
    {
        memset(SmallTextBoxInputText,0,sizeof(SmallTextBoxInputText));
        memset(BigTextBoxInputText,0,sizeof(BigTextBoxInputText));
    }
}
static void DeactivateTextBoxVariables()                        //deactivate all variables
{
    a = b = result = operation = BaseNumberCount = 0;
    ActiveSecondNumber = EnterAlreadyPressed = ErrorCase = false;
}
//-------------------------------------------------------------------------------------------

//Operation Buttons
//-------------------------------------------------------------------------------------------
static void ErrorDivideByZero()//exception function
{
    strcat(SmallTextBoxInputText, "=");
    ClearTextBoxes(BIG_TEXT_BOX);
    strcpy(BigTextBoxInputText, "ERROR");

    DeactivateTextBoxVariables();
    EnterAlreadyPressed = ErrorCase = true;
}

static void SetOperation(const char *string_operation, char char_operation) //standart output for a simple operations
{
    sprintf(SmallTextBoxInputText, "%lld ", a);
    strcat(SmallTextBoxInputText, string_operation);
    operation = char_operation;
    ActiveSecondNumber = true;
    BaseNumberCount=0;

    ClearTextBoxes(BIG_TEXT_BOX); //comment
    BigTextBoxInputText[0] = '0'; //this section
    b=0;                          //if need a temporary calculator history
}

static void ButtonAdition()        { SetOperation("+", '+'); }
static void ButtonSubstraction()   { SetOperation("-", '-'); }
static void ButtonMultiplication() { SetOperation("x", 'x'); }
static void ButtonDivision()       { SetOperation("/", '/'); }

static void SpecialOperationOutput(const char *format_specifier) // output for special operations
{
    strcat(SmallTextBoxInputText, " =");
    sprintf(BigTextBoxInputText, format_specifier, result);
    DeactivateTextBoxVariables();
}

static void EnterSpecialOperation(SpecialException spec_ex, const char *format_specifier) // output result of special operation  in dependence if it will be devided by 0
{
    if(spec_ex == NO_EXCEPTION) SpecialOperationOutput(format_specifier);// ordinary special operations

    if(spec_ex == EXCEPTION)                                            //if it is devided by 0
    {
        if(a) SpecialOperationOutput(format_specifier);
        else ErrorDivideByZero();
    }
}

static void SetSpecialOperation(const char *first_part_string, const char *second_part_string, char special_operation)//in dependence of what operation is setted as argument the result will be that
{
    char number_buffer[NUM_LIMIT+2];
    if(!EnterAlreadyPressed && !ActiveSecondNumber) // operation will be executed if enter is not pressed and second number is not activated
    {
        sprintf(number_buffer, "%lld", a);

        strcat(SmallTextBoxInputText, first_part_string);
        strcat(SmallTextBoxInputText, number_buffer);
        strcat(SmallTextBoxInputText, second_part_string);

        switch(special_operation)
        {
           case '*' :
                    result = a*a;
                    EnterSpecialOperation(NO_EXCEPTION, "%g");
                    break;

            case '^':
                    result = sqrt(a);
                    EnterSpecialOperation(NO_EXCEPTION, "%.14g");
                    break;

            case '\\':
                    result = 1.0f/(double)a;
                    EnterSpecialOperation(EXCEPTION, "%.14g");
                    break;
        }
        EnterAlreadyPressed = true;
    }
}

static void ButtonSqrX()  { SetSpecialOperation("sqr(", ")", '*'); }
static void ButtonSqrtX() { SetSpecialOperation("sqrt(", ")", '^'); }
static void ButtonOneX()  { SetSpecialOperation("1 / (", ")", '\\'); }

static void ButtonPercent()             //calculate the proc and set the proc flag as active and call automatically enter button
{
    if(ActiveSecondNumber)
    {
        proc = ((double)(b*a))/100.0f;
        ActivePercentSign = true;
        ButtonEnter();
    }
}

static void Negation(long long *num)            //gets the number as argument and inverse it, update the bigtextbox and if the number is negative, it adds negative sign and increase base number count for negative sign
{
    *num = -*num;
    sprintf(BigTextBoxInputText, "%lld", *num);
    *num<0?BaseNumberCount++:BaseNumberCount--; //else if the number is converted from negative to positive, base number count is decreased to delete negative sign count
}

static void ButtonPlusMinus()
{
    if((a && !ActiveSecondNumber) && operation == 0) Negation(&a);//if((a && !b) && operation == '\0') Negation(&a); // !ActiveSecondNumber == !b
    if(a && b) Negation(&b);//avoid assigne negative sign to 0, that will lead to undefined behaviour when using backspace
}

//static void ButtonPoint()
//{
//
//}
//-------------------------------------------------------------------------------------------

//Displaying functions
//-------------------------------------------------------------------------------------------
static void PrintResultDeactivateVar()          //print an certain result and deactivate variables in certain cases of PrintEnterResult()
{
    strcat(SmallTextBoxInputText, "=");
    sprintf(BigTextBoxInputText, "%.14g", result);
    DeactivateTextBoxVariables();
    EnterAlreadyPressed = true;
}

static void PrintEnterResult(EnterPhase phase) // print result in dependent of equal cases
{
    if(phase == COMPLETE_ENTER)
        PrintResultDeactivateVar();

    if(phase == INCOMPLETE_ENTER)
    {
        sprintf(SmallTextBoxInputText, "%lld ", a);
        strcat(SmallTextBoxInputText, "=");
        sprintf(BigTextBoxInputText, "%lld", a);
        //DeactivateTextBoxVariables(); //dupa apasarea dubla a enterului se egaleaza automat "a" cu 0 si se printeaza 0
        EnterAlreadyPressed = false;    //dupa apasarea enterului si apoi inceperea tastarii din nou, numarul nu se egaleaza cu 0 si nu se incepe de la inceput
    }

    if(phase == PERCENT_ENTER)
    {
        char proc_buffer[NUM_LIMIT+10];
        char converted_buffer_operation[2];

        sprintf(converted_buffer_operation, "%c", operation);
        sprintf(proc_buffer, " %g ", proc);

        sprintf(SmallTextBoxInputText, "%lld ", a);
        strcat(SmallTextBoxInputText, converted_buffer_operation);
        strcat(SmallTextBoxInputText, proc_buffer);

        PrintResultDeactivateVar();
    }
}

static void EnterPercentSignCase(char enter_case_operation)   //calculate the result in dependence if the percent button was pressed and if this is true then inverse percent flag
{
    if(ActivePercentSign)
    {
        switch (enter_case_operation)
        {
            case '+': result = a+proc; break;
            case '-': result = a-proc; break;
            case 'x': result = a*proc; break;
            case '/': result = (double)a / proc; break;
        }

        ActivePercentSign = !ActivePercentSign;
        PrintEnterResult(PERCENT_ENTER);
    }
    else
    {
        switch(enter_case_operation)
        {
            case '+': result = a+b; break;
            case '-': result = a-b; break;
            case 'x': result = a*b; break;
            case '/': result = (double)a/(double)b; break;
        }
        PrintEnterResult(COMPLETE_ENTER);
    }
}

static void ButtonEnter()                                   //output de corresponding result, and if no operation is pressed the incomplete enter case will be shown
{
    char second_number_buffer[NUM_LIMIT+2];
    if(!EnterAlreadyPressed)
    {
        sprintf(second_number_buffer, " %lld ", b);         //second numer is converted to string and then strcat-ed to rest of the smalltextbox string
        strcat(SmallTextBoxInputText, second_number_buffer);

        switch(operation)
        {
            case '+': EnterPercentSignCase('+'); break;
            case '-': EnterPercentSignCase('-'); break;
            case 'x': EnterPercentSignCase('x'); break;

            case '/':
                    if(b) EnterPercentSignCase('/');
                    else ErrorDivideByZero();
                    break;

            default: PrintEnterResult(INCOMPLETE_ENTER);
        }
    }
}

static void FormNumber(long long *num,char ch)     // formating the number
{
    int limit = *num<0?NUM_LIMIT+1:NUM_LIMIT;      //calculating the limit in dependence of the positive/negative number, cuz of need to display 9 number, but negative sign is counted in base number count

    if(BaseNumberCount < limit)
    {
        int n = ch - '0';
        *num = *num<0?*num*10-n:*num*10+n;
        sprintf(BigTextBoxInputText, "%lld", *num);//makes update of number if it is showed on screen but has value 0 --> ButtonBackSpace (else clause)
        BaseNumberCount++;
    }
}

static void DisplayNumber(char ch)  //number will be formed in depencence of what stage it is, if is for first or second number 
{
    if(!ActiveSecondNumber)
    {
        ClearTextBoxes(SMALL_TEXT_BOX);//clear small text box after another number beyond = sign is introduced
        FormNumber(&a, ch);
    }

    if(ActiveSecondNumber)
        FormNumber(&b, ch);
}
//-------------------------------------------------------------------------------------------


