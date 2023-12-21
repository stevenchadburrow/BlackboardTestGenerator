// Blackboard Test Question Generator 2

// You need to install this first:
// sudo apt-get install libgnuplot-iostream-dev
// Might also need to install 'gnuplot' separately?

// To compile:
// g++ -o BbTestGen2.o BbTestGen2.cpp -lboost_iostreams -lboost_system -lboost_filesystem

// To run:
// ./BbTestGen2.o pools 10
// ./BbTestGen2.o single
// ./BbTestGen2.o custom "<p>Write this expression:</p><p>\\(x^{2}-5x+6=0\\)</p>" "{x}{^{{2}}}{-}{5}{x}{+}{6}{=}{0}"

// Example 'compile':
// compile("<p>Copy this algebraic expression:</p><p>\\(x^{2}-5x+6=0\\)</p><p>$f(x) = sqrt(x-2)+3 ; set object 1 circle at 2,3 size 0.333 fillcolor rgb 'dark-violet' fill solid noborder; set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 ; set ytics axis 1 ; set format x '' ; set format y '' ; set grid xtics ytics ; set xzeroaxis ; set yzeroaxis ; set key noautotitle$</p>", "{x}{^{{2}}}{-}{5}{x}{+}{6}{=}{0}");

// Vertical line in gnuplot:
// set for [i=1:9] linetype i dashtype i ; set arrow from %d,%d to %d,%d nohead lw 2 lc rgb '#009e73' dt 2 ;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "gnuplot-iostream.h"

FILE *output = NULL;
FILE *input = NULL;

int pool_size = 25;
int attempts = 0; // if more than 0, it will produce the 'check answers' section on the toolbox

char content[4096];
char answers[4096];

int bytes = 0;
int pos = 0;

int randomizer = 0;

char file[256];

const char *single_cursor = "\\fbox{$\\hspace{1pt}\\vphantom{12pt}$}"; //"\\square{}";
const char *double_cursor = "{\\\\fbox{$\\\\hspace{1pt}\\\\vphantom{12pt}$}}"; //"{\\\\square{}}";

int A = 0, B = 0, C = 0, D = 0, E = 0, F = 0, G = 0, H = 0, I = 0, J = 0, K = 0, L = 0;

int printheader()
{
	fprintf(output, "<p><iframe class=\"my-iframe\" style=\"border: 1px solid black; overflow-y: scroll; width: 100%%; height: 300px;\"");
	fprintf(output, " onload=\"var pos=-1; for (var i=0; i<document.getElementsByClassName('my-iframe').length; i++) { ");
	fprintf(output, "  if (this.contentDocument == document.getElementsByClassName('my-iframe')[i].contentDocument) { pos = i; break; } } ");
	fprintf(output, "  this.contentDocument.getElementsByClassName('my-input-id')[0].value = pos;");
	fprintf(output, "  if (document.getElementsByClassName('takeQuestionDiv').length == 0) { ");
	fprintf(output, "   this.contentDocument.getElementsByClassName('my-display')[0].style.display = 'none'; ");
	fprintf(output, "   this.contentDocument.getElementsByClassName('my-tables')[0].style.display = 'none'; }");
	fprintf(output, "  if (document.getElementsByClassName('takeQuestionDiv').length == 0) { } else {");
	fprintf(output, "   document.getElementsByClassName('takeQuestionDiv')[pos].getElementsByTagName('input')[0].style.display = 'none';");
	fprintf(output, "   this.contentDocument.getElementsByClassName(`my-input-half`)[0].value = ");
	fprintf(output, "    document.getElementsByClassName('takeQuestionDiv')[pos].getElementsByTagName('input')[0].value;");
	fprintf(output, "   this.contentDocument.getElementsByClassName(`my-input-half`)[0].dispatchEvent(new Event(`input`)); }");
	fprintf(output, "  if (document.getElementsByClassName('questionShell').length == 0) {");
	fprintf(output, "   this.contentDocument.getElementsByClassName('my-answers')[0].style.display = 'none'; }");
	fprintf(output, "  else { this.contentDocument.getElementsByClassName('my-answer-correct')[0].innerHTML = '\\\\(' +");
	fprintf(output, "   document.getElementsByClassName('questionShell')[pos].getElementsByTagName('tr')[2].getElementsByTagName('td')[1].innerHTML.slice(");
	fprintf(output, "   document.getElementsByClassName('questionShell')[pos].getElementsByTagName('tr')[2].getElementsByTagName('td')[1].innerHTML.indexOf(';')+1) + '\\\\)'; ");
	fprintf(output, "   this.contentDocument.getElementsByClassName('my-answer-given')[0].innerHTML = '\\\\(' +");
	fprintf(output, "   document.getElementsByClassName('questionShell')[pos].getElementsByTagName('tr')[0].getElementsByTagName('td')[1].innerHTML.slice(");
	fprintf(output, "   document.getElementsByClassName('questionShell')[pos].getElementsByTagName('tr')[0].getElementsByTagName('td')[1].innerHTML.indexOf(';')+1) + '\\\\)'; } ");
	fprintf(output, "   this.contentDocument.getElementsByClassName('my-input-type')[0].style = 'position:absolute;left:-1000px;width:0px;';");
	fprintf(output, "   window.parent.document.body.onclick = function() { for (var i=0; i!=document.getElementsByClassName('my-iframe').length; i++) {");
	fprintf(output, "   document.getElementsByClassName('my-iframe')[i].contentDocument.getElementsByClassName('my-label')[0].style.boxShadow='0 0 1px 1px black'; } };\"");
	fprintf(output, " srcdoc=\"<body onclick='if(event.target.tagName!=`svg` && event.target.tagName!=`path`) window.document.getElementsByClassName(`my-input-type`)[0].focus();'><button class='my-button-adjust' ");
	fprintf(output, "   onclick='MathJax.typeset(); var pos=-1; for (var i=0; i<window.parent.document.getElementsByClassName(`my-iframe`).length; i++) {");
	fprintf(output, "    if (this == window.parent.document.getElementsByClassName(`my-iframe`)[i].contentDocument.getElementsByClassName(`my-button-adjust`)[0]) { pos = i; break; } }");
	fprintf(output, "    if (window.parent.document.getElementsByClassName(`my-iframe`)[pos].style.height==`300px`) {");
	fprintf(output, "    window.parent.document.getElementsByClassName(`my-iframe`)[pos].style.height = `1000px`; }");
	fprintf(output, "    else { window.parent.document.getElementsByClassName(`my-iframe`)[pos].style.height = `300px`; }'>Adjust Size</button>");
	fprintf(output, "  <script type='text/javascript' id='MathJax-script' async src='https://cdn.jsdelivr.net/npm/mathjax@3/es5/tex-mml-chtml.js'></script>");
	fprintf(output, "  <div class='my-content'>");

	return 1;
};

int printbody()
{
	fprintf(output, "  </div>");
	fprintf(output, "  <div class='my-display' style='white-space:nowrap;'>");
	fprintf(output, "  <button class='my-button-arrowleft' style='display:inline-block;'  title='Keyboard: ArrowLeft'");
	fprintf(output, "   onclick='function myleft(str) {");
	fprintf(output, "     var edit = ``;");
	fprintf(output, "     var pos = str.lastIndexOf(`?`);");
	fprintf(output, "     var next = str.slice(0,pos).lastIndexOf(`:`);");
	fprintf(output, "     if (next < 0) { return str; }");
	fprintf(output, "     else { edit = (str.slice(0,next) + `;` + str.slice(next+1)).replace(`?`,`:`).replace(`;`,`?`); return edit; }");
	fprintf(output, "    }");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-type`)[0].focus();");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value =");
	fprintf(output, "    myleft(window.document.getElementsByClassName(`my-input-full`)[0].value);");
	fprintf(output, "   window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));'>&#8592;</button>");
	fprintf(output, "  <label class='my-label' style='display:inline-block; min-width:150px; border:solid black 1px; box-shadow:0 0 1px 1px black;'  title='Click to use Keyboard'");
	fprintf(output, "   onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus();'>\\(%s\\)</label>", single_cursor);
	fprintf(output, "  <input class='my-input-type' style='display:inline-block; width:0px;'");
	fprintf(output, "   onfocus='for (var i=0; i!=window.parent.document.getElementsByClassName(`my-iframe`).length; i++) { ");
	fprintf(output, "    if (i != window.document.getElementsByClassName(`my-input-id`)[0].value) { ");
	fprintf(output, "    window.parent.document.getElementsByClassName(`my-iframe`)[i].contentDocument.getElementsByClassName(`my-label`)[0].style.boxShadow = `0 0 1px 1px black`; } }");
	fprintf(output, "    window.document.getElementsByClassName(`my-label`)[0].style.boxShadow = `0 0 5px 5px lightblue`;'");
	fprintf(output, "   onkeydown='function mypush(str) {");
	fprintf(output, "     if (window.document.getElementsByClassName(`my-button-` + str.toLowerCase())[0]) {");
	fprintf(output, "     window.document.getElementsByClassName(`my-button-` + str.toLowerCase())[0].click();");
	fprintf(output, "     return true;");
	fprintf(output, "     }");
	fprintf(output, "     else return false;");
	fprintf(output, "    }");
	fprintf(output, "    mypush(event.key);'>");
	fprintf(output, "  <button class='my-button-arrowright' style='display:inline-block;'  title='Keyboard: ArrowRight'");
	fprintf(output, "   onclick='function myright(str) {");
	fprintf(output, "     var edit = ``;");
	fprintf(output, "     var pos = str.lastIndexOf(`?`);");
	fprintf(output, "     var next = str.slice(pos).indexOf(`:`) + pos;");
	fprintf(output, "     if (str.slice(pos).indexOf(`:`) < 0) { return str; }");
	fprintf(output, "     else { edit = (str.slice(0,next) + `;` + str.slice(next+1)).replace(`?`,`:`).replace(`;`,`?`); return edit; }");
	fprintf(output, "    }");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-type`)[0].focus();");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value =");
	fprintf(output, "    myright(window.document.getElementsByClassName(`my-input-full`)[0].value);");
	fprintf(output, "   window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));'>&#8594;</button> ");
	fprintf(output, "  </div>");
	fprintf(output, "  <div class='my-tables'>");
	fprintf(output, "  <table border='1'><tr><td>");
	
	return 1;
};

int printcheck(const char *a)
{
	char correct[4096];

	for (int i=0; i<4096; i++)
	{
		if (a[i] != '\t')
		{
			correct[i] = a[i];
		}
		else
		{
			correct[i] = ';';
		}
	}


	fprintf(output, "  <table border='0' style='width:265px;'><tr><td width='40%%'>");
	fprintf(output, "  <button class='my-button-enter' title='Keyboard: Enter' style='padding:0;width:100%%;'");
	fprintf(output, "   onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus();");
	fprintf(output, "   if (window.document.getElementsByClassName(`my-check-attempts`)[0].value != 1) { ");
	fprintf(output, "   window.document.getElementsByClassName(`my-check-attempts`)[0].value -= 1;");
	fprintf(output, "   var test = false; var arr = window.document.getElementsByClassName(`my-check-answer`)[0].value.split(`;`);");
	fprintf(output, "   for (var i=0; i!=arr.length; i++) { if (window.parent.document.getElementsByClassName(`takeQuestionDiv`)[window.document.getElementsByClassName(`my-input-id`)[0].value].");
	fprintf(output, "   getElementsByTagName(`input`)[0].value == arr[i]) { test = true; } } if (test == true) {");
	fprintf(output, "   window.document.getElementsByClassName(`my-check-label`)[0].innerHTML = `Correct! ` + window.document.getElementsByClassName(`my-check-attempts`)[0].value + ` left`; } else {");
	fprintf(output, "   window.document.getElementsByClassName(`my-check-label`)[0].innerHTML = `Wrong, ` + window.document.getElementsByClassName(`my-check-attempts`)[0].value + ` left`; }} else {");
	fprintf(output, "   window.document.getElementsByClassName(`my-check-label`)[0].innerHTML = `Answer: \\\\(` + window.document.getElementsByClassName(`my-check-answer`)[0].value.split(`;`)[0] + `\\\\)`; MathJax.typeset(); }'>Check Answer</button></td>");
	fprintf(output, "  <td width='60%%'><label class='my-check-label'>%d Checks Left</label></td></tr></table>", attempts);
	fprintf(output, "  <input class='my-check-answer' style='display:none;' value='%s'>", correct);
	fprintf(output, "  <input class='my-check-attempts' style='display:none;' value='%d'>", attempts);

	return 1;
};

int printfooter()
{
	fprintf(output, "  <table border='0' style='width:265px;'>");
	fprintf(output, "  <tr>");
	fprintf(output, "  <td width='10%%'><button class='my-button-0' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus();");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{0}?`);' title='Keyboard: 0'>0</button></td>");
	fprintf(output, "  <td width='10%%'><button class='my-button-1' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{1}?`);' title='Keyboard: 1'>1</button></td>");
	fprintf(output, "  <td width='10%%'><button class='my-button-2' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{2}?`);' title='Keyboard: 2'>2</button></td>");
	fprintf(output, "  <td width='10%%'><button class='my-button-3' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{3}?`);' title='Keyboard: 3'>3</button></td>");
	fprintf(output, "  <td width='10%%'><button class='my-button-4' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{4}?`);' title='Keyboard: 4'>4</button></td>");
	fprintf(output, "  <td width='10%%'><button class='my-button-5' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{5}?`);' title='Keyboard: 5'>5</button></td>");
	fprintf(output, "  <td width='10%%'><button class='my-button-6' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{6}?`);' title='Keyboard: 6'>6</button></td>");
	fprintf(output, "  <td width='10%%'><button class='my-button-7' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{7}?`);' title='Keyboard: 7'>7</button></td>");
	fprintf(output, "  <td width='10%%'><button class='my-button-8' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{8}?`);' title='Keyboard: 8'>8</button></td>");
	fprintf(output, "  <td width='10%%'><button class='my-button-9' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{9}?`);' title='Keyboard: 9'>9</button></td>");
	fprintf(output, "  </tr>");
	fprintf(output, "  </table>");
	fprintf(output, "  <table border='0' style='width:265px;'>");
	fprintf(output, "  <tr>");
	fprintf(output, "  <td width='10%%'><button class='my-button-(' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{(}?`);' title='Keyboard: ('>(</button></td>");
	fprintf(output, "  <td width='10%%'><button class='my-button-)' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{)}?`);' title='Keyboard: )'>)</button></td>");
	fprintf(output, "  <td width='10%%'><button class='my-button-[' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{[}?`);' title='Keyboard: ['>[</button></td>");
	fprintf(output, "  <td width='10%%'><button class='my-button-]' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{]}?`);' title='Keyboard: ]'>]</button></td>");
	fprintf(output, "  <td width='10%%'><button class='my-button-u' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{\\\\cup}?`);' title='Keyboard: u'>&cup;</button></td>");
	fprintf(output, "  <td width='10%%'><button class='my-button-i' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{\\\\infty}?`);' title='Keyboard: i'>&infin;</button></td>");
	fprintf(output, "  <td width='10%%'><button class='my-button-e' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{e}?`);' title='Keyboard: e'><i>e</i></button></td>");
	fprintf(output, "  <td width='10%%'><button class='my-button-.' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{.}?`);' title='Keyboard: .'>.</button></td>");
	fprintf(output, "  <td width='10%%'><button class='my-button-,' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{,}?`);' title='Keyboard: ,'>,</button></td>");
	fprintf(output, "  <td width='10%%'><button class='my-button-|' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{|}?`);' title='Keyboard: |'>|</button></td>");
	fprintf(output, "  </tr>");
	fprintf(output, "  </table>");
	fprintf(output, "  <table border='0' style='width:265px;'>");
	fprintf(output, "  <tr>");
	fprintf(output, "  <td width='10%%'><button class='my-button-a' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{a}?`);' title='Keyboard: a'>a</button></td>");
	fprintf(output, "  <td width='10%%'><button class='my-button-b' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{b}?`);' title='Keyboard: b'>b</button></td>");
	fprintf(output, "  <td width='10%%'><button class='my-button-c' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{c}?`);' title='Keyboard: c'>c</button></td>");
	fprintf(output, "  <td width='10%%'><button class='my-button-d' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{d}?`);' title='Keyboard: d'>d</button></td>");
	fprintf(output, "  <td width='10%%'><button class='my-button-x' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{x}?`);' title='Keyboard: x'>x</button></td>");
	fprintf(output, "  <td width='10%%'><button class='my-button-y' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{y}?`);' title='Keyboard: y'>y</button></td>");
	fprintf(output, "  <td width='10%%'><button class='my-button-z' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{z}?`);' title='Keyboard: z'>z</button></td>");
	fprintf(output, "  <td width='10%%'><button class='my-button-+' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{+}?`);' title='Keyboard: +'>+</button></td>");
	fprintf(output, "  <td width='10%%'><button class='my-button--' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{-}?`);' title='Keyboard: -'>-</button></td>");
	fprintf(output, "  <td width='10%%'><button class='my-button-p' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{\\\\pm}?`);' title='Keyboard: p'>&#177;</button></td>");
	fprintf(output, "  </tr>");
	fprintf(output, "  </table>");
	fprintf(output, "  <table border='0' style='width:265px;'>");
	fprintf(output, "  <tr>");
	fprintf(output, "  <td width='10%%'><button class='my-button-=' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{=}?`);' title='Keyboard: ='>=</button></td>");
	fprintf(output, "  <td width='10%%'><button class='my-button-/' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{\\\\dfrac{?}{:}}:`);' title='Keyboard: /'>&#247;</button></td>");
	fprintf(output, "  <td width='10%%'><button class='my-button-^' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{^{?}}:`);' title='Keyboard: ^'>^</button></td>");
	fprintf(output, "  <td width='10%%'><button class='my-button-s' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{\\\\sqrt{?}}:`);' title='Keyboard: s'>&radic;</button></td>");
	fprintf(output, "  <td width='10%%'><button class='my-button-r' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{\\\\sqrt[3]{?}}:`);' title='Keyboard: r'>&#8731;</button></td>");
	fprintf(output, "  <td width='10%%'><button class='my-button-l' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{\\\\log_{?}({:})}:`);' title='Keyboard: l'><small>log</small></button></td>");
	fprintf(output, "  <td width='10%%'><button class='my-button-n' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{\\\\ln({?})}:`);' title='Keyboard: n'><small>ln</small></button></td>");
	fprintf(output, "  <td width='15%%'><button class='my-button-m' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{\\\\begin{bmatrix}{?}&{:}\\\\\\\\{:}&{:}\\\\end{bmatrix}}:`);' title='Keyboard: m'><small>2x2</small></button></td>");
	fprintf(output, "  <td width='15%%'><button class='my-button-q' style='padding:0;width:100%%;' onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus(); ");
	fprintf(output, "   function myrep(str) {");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "     window.document.getElementsByClassName(`my-input-full`)[0].value.replace(`?`,str); ");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));");
	fprintf(output, "   }");
	fprintf(output, "   myrep(`:{\\\\begin{bmatrix}{?}&{:}&{:}\\\\\\\\{:}&{:}&{:}\\\\\\\\{:}&{:}&{:}\\\\end{bmatrix}}:`);' title='Keyboard: q'><small>3x3</small></button></td>");
	fprintf(output, "  </tr>");
	fprintf(output, "  </table>");
	fprintf(output, "  <table border='0' style='width:265px;'>");
	fprintf(output, "  <tr>");
	fprintf(output, "  <td width='33%%'><button class='my-button-backspace' style='padding:0;width:100%%;'  title='Keyboard: Backspace'");
	fprintf(output, "   onclick='function myback(str) {");
	fprintf(output, "     var edit = ``;");
	fprintf(output, "     var pos = str.lastIndexOf(`?`) - 1;");
	fprintf(output, "     var next = -1;");
	fprintf(output, "     if (str.charAt(pos) == `{`) return str;");
	fprintf(output, "     else {");
	fprintf(output, "      var count = 1;");
	fprintf(output, "      for (var i=pos-1; i>=0; i--) {");
	fprintf(output, "      if (str.charAt(i) == `{`) count--;");
	fprintf(output, "      if (str.charAt(i) == `}`) count++;");
	fprintf(output, "      if (count == 0) { next = i; break; }");
	fprintf(output, "     }");
	fprintf(output, "     if (next == -1) return str;");
	fprintf(output, "     else { edit = str.slice(0,next-1) + str.slice(pos+1); return edit; } ");
	fprintf(output, "     }");
	fprintf(output, "    }");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-type`)[0].focus();");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "    myback(window.document.getElementsByClassName(`my-input-full`)[0].value); ");
	fprintf(output, "   window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));'>Backspace</button></td>");
	fprintf(output, "  <td width='33%%'><button class='my-button-delete' style='padding:0;width:100%%;'  title='Keyboard: Delete'");
	fprintf(output, "   onclick='function mydel(str) {");
	fprintf(output, "     var edit = ``;");
	fprintf(output, "     var pos = str.lastIndexOf(`?`) + 1;");
	fprintf(output, "     var next = -1;");
	fprintf(output, "     if (str.charAt(pos) == `}`) return str;");
	fprintf(output, "     else {");
	fprintf(output, "      var count = 1;");
	fprintf(output, "      for (var i=pos+1; i<str.length; i++) {");
	fprintf(output, "      if (str.charAt(i) == `{`) count++;");
	fprintf(output, "      if (str.charAt(i) == `}`) count--;");
	fprintf(output, "      if (count == 0) { next = i; break; }");
	fprintf(output, "     }");
	fprintf(output, "     if (next == -1) return str;");
	fprintf(output, "     else { edit = str.slice(0,pos) + str.slice(next+2); return edit; }");
	fprintf(output, "     }");
	fprintf(output, "    }");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-type`)[0].focus();");
	fprintf(output, "    window.document.getElementsByClassName(`my-input-full`)[0].value = ");
	fprintf(output, "    mydel(window.document.getElementsByClassName(`my-input-full`)[0].value); ");
	fprintf(output, "   window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));'>Delete</button></td>");
	fprintf(output, "  <td width='33%%'><button class='my-button-escape' style='padding:0;width:100%%;'  title='Keyboard: Escape'");
	fprintf(output, "   onclick='window.document.getElementsByClassName(`my-input-type`)[0].focus();");
	fprintf(output, "   window.document.getElementsByClassName(`my-input-full`)[0].value = `?`;");
	fprintf(output, "   window.document.getElementsByClassName(`my-input-full`)[0].dispatchEvent(new Event(`input`));'>Clear</button></td>");
	fprintf(output, "  </tr>");
	fprintf(output, "  </table>");
	fprintf(output, "  <input class='my-input-full' value='?' style='display:none;' ");
	fprintf(output, "   oninput='window.document.getElementsByClassName(`my-input-half`)[0].value = ");
	fprintf(output, "    this.value.replace(`?`,`%s`).replace(/[: ]/g,``).toLowerCase();", double_cursor);
	fprintf(output, "    window.document.getElementsByClassName(`my-input-half`)[0].dispatchEvent(new Event(`input`));'>");
	fprintf(output, "  <input class='my-input-half' style='display:none;'");
	fprintf(output, "   oninput='window.document.getElementsByClassName(`my-label`)[0].innerHTML = ");
	fprintf(output, "    `\\\\(` + (this.value.length == 0 ? `%s` : ` `) + this.value + `\\\\)`; ", double_cursor);
	fprintf(output, "   window.parent.document.getElementsByClassName(`takeQuestionDiv`)[window.document.getElementsByClassName(`my-input-id`)[0].value].getElementsByTagName(`input`)[0].value = ");
	fprintf(output, "    this.value.replace(`%s`,``); MathJax.typeset();'>", double_cursor);
	fprintf(output, "  <input class='my-input-id' style='display:none;' value='-1'>");
	fprintf(output, "  </td></tr></table>");
	fprintf(output, "  </div>");
	fprintf(output, "  <div class='my-answers'>");
	fprintf(output, "  <button onclick='MathJax.typeset();'>Show Math-Type Answers</button>");
	fprintf(output, "  <p>Given: <label class='my-answer-given'></label></p>");
	fprintf(output, "  <p>Correct: <label class='my-answer-correct'></label></p>");
	fprintf(output, "  </div>");
	fprintf(output, "  </body>");
	fprintf(output, "\"></iframe></p>");
	
	return 1;
};

int fcreate(const char *n)
{
	output = NULL;

	output = fopen(n, "wt");
	
	if (!output)
	{
		printf("Error in %s\n", n);
	
		return 0;
	}

	return 1;
};

int copysvg(const char *n)
{
	input = NULL;

	input = fopen(n, "rt");
	
	if (!input)
	{
		printf("Error in %s\n", n);

		return 0;
	}

	int bytes = 1;
	char buffer = 0;

	int sequence = 0;

	while (bytes > 0)
	{
		bytes = fscanf(input, "%c", &buffer);

		if (buffer == '"') buffer = '\''; // replace " with '
		if (buffer == '\t') buffer = ' '; // replace \t with (space)
		if (buffer == '\n') buffer = ' '; // replace \n with (space)
	
		fprintf(output, "%c", buffer);
		
		if (sequence == 3 && buffer == 'g')
		{
			fprintf(output, " onclick='if(this.getAttribute(`width`)==`250`){this.setAttribute(`width`,`500`); this.setAttribute(`height`,`400`); this.setAttribute(`viewbox`,`0 0 500 400`);}");
			fprintf(output, "else{this.setAttribute(`width`,`250`); this.setAttribute(`height`,`200`); this.setAttribute(`viewbox`,`0 0 250 200`);}'");

			sequence = 0;
		}
		else if (sequence == 2 && buffer == 'v') sequence++;
		else if (sequence == 1 && buffer == 's') sequence++;
		else if (sequence == 0 && buffer == '<') sequence++;
		else sequence = 0;
	}

	fclose(input);

	return 1;
};

bool isprime(const int v)
{
	bool test = true;

	if (v == 1) return false;

	for (int l=v-1; l>1; l--)
	{
		if (v % l == 0) test = false;
	}

	return test;
};

bool isinteger(const float v)
{
	if ((float)(v - (int)(v)) == 0.0f) return true;
	else return false;
};

int factorial(const int v)
{
	int t = 1;
	
	for (int i=1; i<=v; i++)
	{
		t *= i;
	}

	return t;
};

// c = command line for gnuplot
int graph(const char *c)
{	
	char command[1024];

	for (int i=0; i<1024; i++) command[i] = 0;

	sprintf(command, "%s graph \"f(x)=1000000 ; g(x)=1000000 ; %s ; set term svg size 250,200 ; set output 'Graph.svg' ; plot f(x),g(x)\n\"", file, c);

	system(command); // creates 'Graph.svg'

	if (!copysvg("Graph.svg"))
	{
		return 0;
	}

	//system("rm Graph.svg"); // removes 'Graph.svg'

	return 1;
};

// s = string of single characters, usually pre-formatted answers
// r = result string
// This will only format values not already in { }
int format(const char *s, char *r)
{
	char changed[4096];

	for (int i=0; i<4096; i++) changed[i] = 0;

	int bytes = 0;
	int pos = 0;

	int inside = 0;
	bool outside = false;

	for (int i=0; i<strlen(s); i++)
	{
		if (s[i] != '\t')
		{
			if (s[i] != ' ')
			{
				if (s[i] == '{') inside++;

				if (s[i] == '`') outside = !outside;

				if (inside == 0 || outside == true)
				{
					if (s[i] != '`')
					{
						bytes = sprintf(changed + pos, "{%c}", s[i]);
						pos += bytes;
					}
				}
				else
				{
					if (s[i] != '`')
					{
						bytes = sprintf(changed + pos, "%c", s[i]);
						pos += bytes;
					}
				}
			
				if (s[i] == '}') inside--;
			}
		}
		else
		{
			bytes = sprintf(changed + pos, "%c", s[i]);
			pos += bytes;
		}
	}

	strcpy(r, changed);	

	return 1;
};

// s = content string with gnuplot inside of @...@
// a = answers, tab separated
int compile(const char *s, const char *a)
{
	fprintf(output, "FIB\t");

	printheader();

	for (int i=0; i<strlen(s); i++)
	{
		if (s[i] == '@')
		{
			char command[1024];

			for (int j=0; j<1024; j++) command[j] = 0;

			for (int j=0; j<strlen(s)-i-1; j++)
			{
				if (s[i+j+1] != '@')
				{
					command[j] = s[i+j+1];
				}
				else
				{
					if (!graph(command))
					{
						return 0;
					}

					i += j + 1;

					break;
				}
			}
		}
		else
		{
			fprintf(output, "%c", s[i]);
		}
	}

	printbody();

	if (attempts > 0) printcheck(a);

	printfooter();

	fprintf(output, "\t%s\n", a);

	return 1;
};

int question_1_01(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 19 - 9;
			B = rand() % 19 - 9;
			C = rand() % 48 + 2;
			D = rand() % 48 + 2;
		}
		while ((float)(A*B-D*A) == 0.0f || !isinteger((float)(C * A + B) / (float)(A * B - D * A)) || (fabs((float)(C * A + B) / (float)(A * B - D * A))) >= 100.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;
	
		bytes = sprintf(content + pos, "<p>Evaluate the algebraic expression.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>For \\(x = %d\\) and \\(y = %d\\),</p>", A, B); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(\\dfrac{ %dx+y }{ xy-%dx } =\\ \\ ?\\)</p>", C, D); pos += bytes;
			
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos, "%d", (int)((float)(C * A + B) / (float)(A * B - D * A))); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_1_02(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 19 - 9;
			B = rand() % 19 - 9;
		}
		while (A == 0 || B == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;
	
		bytes = sprintf(content + pos, "<p>Rewrite the expression without absolute value bars.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(||%d|-|%d|| =\\ \\ ?\\)</p>", A, B); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos, "%d", (int)fabs(fabs(A) - fabs(B))); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_1_03(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 8 + 2;
			B = rand() % 8 + 2;
			C = rand() % 9 + 1;
			D = rand() % 9 + 1;
		}
		while (A*B-B == 0 || -A*C-D == 0 || A*B-B == 1 || A*B-B == -1);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Simplify the given algebraic expression.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(%d(%dy-%d)-(%dy+%d) =\\ \\ ?\\)</p>", A, B, C, B, D); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos, "%dy%c%d\t", A*B-B, (-A*C-D>=0?'+':'-'), (int)fabs(-A*C-D)); pos += bytes;
		bytes = sprintf(answers + pos, "%d%c%dy", -A*C-D, (A*B-B>=0?'+':'-'), (int)fabs(A*B-B)); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_1_04(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 9 + 1;
			B = rand() % 8 + 2;
			C = rand() % 9 + 1;
			D = rand() % 8 + 2;
		}
		while (B*D == 0 || A-B*C-B*A == 0 || B*D == 1 || B*D == -1);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Simplify the given algebraic expression.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(%d-%d[%d-(%dy-%d)] =\\ \\ ?\\)</p>", A, B, C, D, A); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos, "%dy%c%d\t", B*D, (A-B*C-B*A>=0?'+':'-'), (int)fabs(A-B*C-B*A)); pos += bytes;
		bytes = sprintf(answers + pos, "%d%c%dy", A-B*C-B*A, (B*D>=0?'+':'-'), (int)fabs(B*D)); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_1_05(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 8 + 2;
			B = rand() % 8 + 2;
			C = rand() % 9 + 1;
			D = rand() % 9 + 1;
		}
		while (A-B == 0 || B*C-D+1 == 0 || A-B == 1 || A-B == -1);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Simplify the given algebraic expression.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(%dx+1-[%d(x-%d)+%d] =\\ \\ ?\\)</p>", A, B, C, D); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos, "%dx%c%d\t", A-B, (B*C-D+1>=0?'+':'-'), (int)fabs(B*C-D+1)); pos += bytes;
		bytes = sprintf(answers + pos, "%d%c%dx", B*C-D+1, (A-B>=0?'+':'-'), (int)fabs(A-B)); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_1_06(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		A = 20 + rand() % 9 + 1;
		B = 800 + rand() % 99 + 1;
		C = 15000 + rand() % 999 + 1;
		D = rand() % 9 + 1;

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>The formula models the average cost of tuition and fees, \\(T\\), "); pos += bytes;
		bytes = sprintf(content + pos, "at private colleges for the school year ending \\(x\\) years after 2010.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(T=%dx^{2}+%dx+%d\\)</p>", A, B, C); pos += bytes;
		bytes = sprintf(content + pos, "<p>Using the formula, find the average cost of tuition and fees "); pos += bytes;
		bytes = sprintf(content + pos, "at private colleges for the school year ending in %d.</p>", 2010 + D); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos, "%d", A*D*D+B*D+C); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_1_07(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		A = 20 + rand() % 9 + 1;
		B = 800 + rand() % 99 + 1;
		C = 15000 + rand() % 999 + 1;
		D = rand() % 9 + 1;
		E = 6 * (rand() % 50 + 1);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>The formula models the average cost of tuition and fees, \\(T\\), "); pos += bytes;
		bytes = sprintf(content + pos, "at private colleges for the school year ending \\(x\\) years after 2010.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(T=%dx^{2}+%dx+%d\\)</p>", A, B, C); pos += bytes;
		bytes = sprintf(content + pos, "<p>The actual values are listed below:</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p><table border='1'><tr><th>Year</th><th>Cost</th></tr>"); pos += bytes;
		bytes = sprintf(content + pos, "<tr><td>%d</td><td>%d</td></tr>", 2010+D-2, A*(D-2)*(D-2)+B*(D-2)+C-E/3); pos += bytes;
		bytes = sprintf(content + pos, "<tr><td>%d</td><td>%d</td></tr>", 2010+D-1, A*(D-1)*(D-1)+B*(D-1)+C-E/2); pos += bytes;
		bytes = sprintf(content + pos, "<tr><td>%d</td><td>%d</td></tr>", 2010+D, A*(D)*(D)+B*(D)+C-E); pos += bytes;
		bytes = sprintf(content + pos, "<tr><td>%d</td><td>%d</td></tr>", 2010+D+1, A*(D+1)*(D+1)+B*(D+1)+C-E/2); pos += bytes;
		bytes = sprintf(content + pos, "</table></p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>Find how much the formula overestimates the actual value of the average cost of "); pos += bytes;
		bytes = sprintf(content + pos, "tuition and fees at private colleges for the school year ending in %d.</p>", 2010 + D); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos, "%d", E); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_1_08(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		A = rand() % 8 + 2;
		B = rand() % 8 + 2;

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Simplify the given exponential expression.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(x^{%d}\\cdot x^{%d} =\\ \\ ?\\)</p>", A, B); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos, "x{^{`%d`}}", A+B); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_1_09(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		A = rand() % 8 + 2;
		B = rand() % 8 + 2;

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Simplify the given exponential expression.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\((x^{%d})^{%d} =\\ \\ ?\\)</p>", A, B); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos, "x{^{`%d`}}", A*B); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_1_10(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 8 + 2;
			B = rand() % 8 + 2;
		}
		while (A-B <= 1);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Simplify the given exponential expression.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(\\dfrac{ y^{%d} }{ y^{%d} } =\\ \\ ?\\)</p>", A, B); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos, "y{^{`%d`}}", A-B); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_1_11(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		A = rand() % 3 + 2;
		B = rand() % 8 + 2;
		C = rand() % 3 + 2;

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Simplify the given exponential expression.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\((%dy^{%d})^{%d} =\\ \\ ?\\)</p>", A, B, C); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos, "%dy{^{`%d`}}", (int)pow(A,C), B*C); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_1_12(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		A = rand() % 3 + 2;
		B = rand() % 8 + 2;
		C = rand() % 8 + 2;
		D = rand() % 3 + 2;

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Simplify the given exponential expression.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\((-%dx^{%d}y^{%d})^{%d} =\\ \\ ?\\)</p>", A, B, C, D); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos, "%dx{^{`%d`}}y{^{`%d`}}\t", (D%2==0?1:-1)*(int)pow(A,D), B*D, C*D); pos += bytes;
		bytes = sprintf(answers + pos, "%dy{^{`%d`}}x{^{`%d`}}", (D%2==0?1:-1)*(int)pow(A,D), C*D, B*D); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_1_13(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 8 + 2;
			B = rand() % 8 + 2;
			C = rand() % 8 + 2;
			D = rand() % 8 + 2;
			E = rand() % 8 + 2;
			F = rand() % 8 + 2;
		}
		while (!isinteger((float)A/(float)D) || B <= E || C <= F || B - D <= 1 || C - D <= 1 || A/D == 1);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Simplify the given exponential expression.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(\\dfrac{ -%d x^{%d} y^{%d} }{ %d x^{%d} y^{%d} } =\\ \\ ?\\)</p>", A, B, C, D, E, F); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos, "-%dx{^{`%d`}}y{^{`%d`}}\t", A/D, B-E, C-F); pos += bytes;
		bytes = sprintf(answers + pos, "-%dy{^{`%d`}}x{^{`%d`}}", A/D, C-F, B-E); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_1_14(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 19 - 9;
			B = rand() % 19 - 9;
		}
		while (fabs(A) <= 1.0f || fabs(B) <= 1.0f || fabs(A*B) <= 1.0f || A == B);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Factor the following by grouping.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(x^{2}%c%dx%c%dx%c%d =\\ \\ ?\\)</p>", 
			(B>0?'+':'-'), (int)fabs(B), (A>0?'+':'-'), (int)fabs(A), (A*B>0?'+':'-'), (int)fabs(A*B)); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos, "(x%c%d)(x%c%d)\t", (A>0?'+':'-'), (int)fabs(A), (B>0?'+':'-'), (int)fabs(B)); pos += bytes;
		bytes = sprintf(answers + pos, "(x%c%d)(x%c%d)", (B>0?'+':'-'), (int)fabs(B), (A>0?'+':'-'), (int)fabs(A)); pos += bytes;
	

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_1_15(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 19 - 9;
			B = rand() % 19 - 9;
		}
		while (fabs(A) <= 1.0f || fabs(B) <= 1.0f || fabs(A+B) <= 1.0f || fabs(A*B) <= 1.0f || A == B);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Factor the given polynomial.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(x^{2}%c%dx%c%d =\\ \\ ?\\)</p>", 
			(A+B>0?'+':'-'), (int)fabs(A+B), (A*B>0?'+':'-'), (int)fabs(A*B)); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos, "(x%c%d)(x%c%d)\t", (A>0?'+':'-'), (int)fabs(A), (B>0?'+':'-'), (int)fabs(B)); pos += bytes;
		bytes = sprintf(answers + pos, "(x%c%d)(x%c%d)", (B>0?'+':'-'), (int)fabs(B), (A>0?'+':'-'), (int)fabs(A)); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_1_16(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 6 + 2;
			B = rand() % 19 - 9;
			C = rand() % 19 - 9;
		}
		while (A == 4 || A == 6 || B == 0 || C == 0 || fabs(A*C+B) <= 1.0f || fabs(B*C) <= 1.0f || 
			isinteger((float)B/(float)A) || isinteger((float)B/(float)C));

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Factor the trinomial.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(%dz^{2}%c%dz%c%d =\\ \\ ?\\)</p>", 
			A, (A*C+B>0?'+':'-'), (int)fabs(A*C+B), (B*C>0?'+':'-'), (int)fabs(B*C)); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos, "(%dz%c%d)(z%c%d)\t", A, (B>0?'+':'-'), (int)fabs(B), (C>0?'+':'-'), (int)fabs(C)); pos += bytes;
		bytes = sprintf(answers + pos, "(z%c%d)(%dz%c%d)", (C>0?'+':'-'), (int)fabs(C), A, (B>0?'+':'-'), (int)fabs(B)); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_1_17(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 8 + 2;
			B = rand() % 8 + 2;
		}
		while (A == 4 || A == 6 || A == B || isinteger((float)B/(float)A) || isinteger((float)A/(float)B));
		

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Factor the difference of two squares.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(%dx^{2}-%dy^{2} =\\ \\ ?\\)</p>", (int)pow(A,2), (int)pow(B,2)); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos, "(%dx-%dy)(%dx+%dy)\t", A, B, A, B); pos += bytes;
		bytes = sprintf(answers + pos, "(%dx+%dy)(%dx-%dy)", A, B, A, B); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_1_18(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 19 - 9;
		}
		while (fabs(A) <= 1.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Factor the following perfect square trinomial.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(z^{2}%c%dz+%d =\\ \\ ?\\)</p>", (A>0?'+':'-'), (int)fabs(2*A), (int)pow(A,2)); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos, "(z%c%d){^{`2`}}\t", (A>0?'+':'-'), (int)fabs(A)); pos += bytes;
		bytes = sprintf(answers + pos, "(z%c%d)(z%c%d)", (A>0?'+':'-'), (int)fabs(A), (A>0?'+':'-'), (int)fabs(A)); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_1_19(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 8 + 2;
			B = rand() % 19 - 9;
			C = rand() % 19 - 9;
		}
		while (fabs(A*B+A*C) <= 1.0f || A*B*C == 0 || B == C || !isprime((int)fabs(B)) || !isprime((int)fabs(C)));

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Factor the expression completely.</p>"); pos += bytes;
		bytes = sprintf(content + pos,  "<p>\\(%dx^{2}%c%dx%c%d,\\ =\\ \\ ?\\)</p>", 
			A, (A*B+A*C>0?'+':'-'), (int)fabs(A*B+A*C), (A*B*C>0?'+':'-'), (int)fabs(A*B*C)); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos, "%d(x%c%d)(x%c%d)\t", A, (B>0?'+':'-'), (int)fabs(B), (C>0?'+':'-'), (int)fabs(C)); pos += bytes;
		bytes = sprintf(answers + pos, "%d(x%c%d)(x%c%d)", A, (C>0?'+':'-'), (int)fabs(C), (B>0?'+':'-'), (int)fabs(B)); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_1_20(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 8 + 2;
			B = rand() % 9 + 1;
			C = rand() % 9 + 1;
			D = rand() % 8 + 2;
			E = rand() % 9 + 1;
		}
		while (!isinteger((float)(D*E+A*B-C)/(float)(A-D)) || A-D == 0 || fabs((float)(D*E+A*B-C)/(float)(A-D)) >= 100.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve and check the linear equation.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(%d(x-%d)+%d=%d(x+%d)\\)</p><p>\\(x =\\ \\ ?\\)</p>", A, B, C, D, E); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos, "%d", (D*E+A*B-C)/(A-D)); pos += bytes;
	
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_1_21(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 19 - 9;
			B = rand() % 8 + 2;
			C = rand() % 9 + 1;
			D = rand() % 9 + 1;
		}
		while (!isinteger((float)(A+B*C-D)/(float)(B-1)) || B-1 == 0 || fabs((float)(A+B*C-D)/(float)(B-1)) >= 100.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve and check the linear equation.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(%d=%d(x-%d)-(x-%d)\\)</p><p>\\(x =\\ \\ ?\\)</p>", A, B, C, D); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos, "%d", (A+B*C-D)/(B-1)); pos += bytes;
	
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_1_22(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 8 + 2;
			B = rand() % 8 + 2;
			C = rand() % 19 - 9;
		}
		while (!isinteger((float)(A*B*C)/(float)(B-A)) || B-A == 0 || fabs((float)(A*B*C)/(float)(B-A)) >= 100.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve and check the equation.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(\\dfrac{ x }{ %d }=\\dfrac{ x }{ %d }%c%d\\)</p><p>\\(x =\\ \\ ?\\)</p>", 
			A, B, (C>0?'+':'-'), (int)fabs(C)); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos, "%d", (A*B*C)/(B-A)); pos += bytes;
	
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_1_23(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 9 + 1;
			B = rand() % 8 + 2;
			C = rand() % 19 + 1;
			D = rand() % 9 + 1;
			E = rand() % 8 + 2;
		}
		while ((float)(B+E) == 0.0f || !isinteger((float)(B*C*E-A*E-B*D)/(float)(B+E)) || fabs((float)(B*C*E-A*E-B*D)/(float)(B+E)) >= 100.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve the equation.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(\\dfrac{ x+%d }{ %d }=%d-\\dfrac{ x+%d }{ %d }\\)</p><p>\\(x =\\ \\ ?\\)</p>", 
			A, B, C, D, E); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos, "%d", (B*C*E-A*E-B*D)/(B+E)); pos += bytes;
	
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_1_24(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 8 + 2;
			B = rand() % 8 + 2;
			C = rand() % 9 + 1;
			D = rand() % 19 - 9;
			E = rand() % 8 + 2;
			F = rand() % 8 + 2;
			G = rand() % 9 + 1;
			H = rand() % 19 - 9;
		}
		while (!isinteger((float)(A*C-D-G*E+H)/(float)(A*B-E*F)) || fabs(A*B-E*F) <= 1.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve the equation.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(%d(%dx-%d)%c%d=%d(%dx-%d)%c%d\\)</p><p>\\(x =\\ \\ ?\\)</p>", 
			A, B, C, (D>0?'+':'-'), (int)fabs(D),  E, F, G, (H>0?'+':'-'), (int)fabs(H)); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos, "%d", (A*C-D-G*E+H)/(A*B-E*F)); pos += bytes;
	
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_1_25(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 8 + 2;
			B = rand() % 9 + 1;
			C = rand() % 8 + 2;
			D = rand() % 19 - 9;
		}
		while (!isinteger((float)(A*B+C*D)/(float)(2*A-C)) || fabs(2*A-C) <= 1.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve the equation such that \\(y = 0\\).</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(y = %d[x-(%d-x)]-%d(x%c%d)\\)</p><p>\\(x =\\ \\ ?\\)</p>", 
			A, B, C, (D>0?'+':'-'), (int)fabs(D)); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos, "%d", (A*B+C*D)/(2*A-C)); pos += bytes;
	
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_1_26(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 19 - 9;
			B = rand() % 19 - 9;
		}
		while (A == 0 || B == 0 || fabs(A+B) <= 1.0f || fabs(A*B) <= 1.0f || A == B);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve by factoring.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(x^{2} = %dx%c%d, x = \\ \\ ?\\)</p>", 
			-A-B, (-A*B>0?'+':'-'), (int)fabs(-A*B)); pos += bytes;
		bytes = sprintf(content + pos, "<p>Separate answers with a comma.</p>"); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%d,%d\t", -A, -B); pos += bytes;
		bytes = sprintf(answers + pos,  "%d,%d", -B, -A); pos += bytes;
	
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_1_27(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 19 - 9;
			B = rand() % 8 + 2;
		}
		while (A == 0 || B == 0 || fabs(B-A) <= 1.0f || fabs(-B-A) <= 1.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve the equation by the square root property.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\((x%c%d)^{2} = %d, x = \\ \\ ?\\)</p>", 
			(A>0?'+':'-'), (int)fabs(A), (int)pow(B,2)); pos += bytes;
		bytes = sprintf(content + pos, "<p>Separate answers with a comma.</p>"); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%d{\\pm}%d\t", -A, B); pos += bytes;
		bytes = sprintf(answers + pos,  "{\\pm}%d%c%d\t", B, (A>0?'-':'+'), (int)fabs(A)); pos += bytes;
		bytes = sprintf(answers + pos,  "%d,%d\t", B-A, -B-A); pos += bytes;
		bytes = sprintf(answers + pos,  "%d,%d",  -B-A, B-A); pos += bytes;
	
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_1_28(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 19 - 9;
			B = rand() % 499 + 2;
			C = rand() % 499 + 2;
		}	
		while (fabs(A) <= 1.0f || isprime(B) == false || isprime(C) == false || B == C || !isinteger((float)((pow(A,2)-B)/4)) || fabs((float)((pow(A,2)-B)/4)) <= 1.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve the equation using quadratic formula.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(x^{2}%c%dx%c%d = 0, x\\ =\\ \\ ?\\)</p>", 
			(-A>0?'+':'-'), (int)fabs(-A), ((int)((pow(A,2)-B)/4)>0?'+':'-'), (int)fabs((pow(A,2)-B)/4)); pos += bytes;
		bytes = sprintf(content + pos, "<p>Separate answers with a comma.</p>"); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "{\\dfrac{`%d`{\\pm}{\\sqrt{`%d`}}}{`2`}}\t", A, B); pos += bytes;
		bytes = sprintf(answers + pos,  "{\\dfrac{`%d+`{\\sqrt{`%d`}}}{`2`}},{\\dfrac{`%d-`{\\sqrt{`%d`}}}{`2`}}\t", A, B, A, B); pos += bytes;
		bytes = sprintf(answers + pos,  "{\\dfrac{`%d-`{\\sqrt{`%d`}}}{`2`}},{\\dfrac{`%d+`{\\sqrt{`%d`}}}{`2`}}\t", A, B, A, B); pos += bytes;
		bytes = sprintf(answers + pos,  "{\\dfrac{{\\pm}{\\sqrt{`%d`}}`%c%d`}{`2`}}\t", B, (A>=0?'+':'-'), (int)fabs(A)); pos += bytes;
		bytes = sprintf(answers + pos,  "{\\dfrac{`+`{\\sqrt{`%d`}}`%c%d`}{`2`}},{\\dfrac{`-`{\\sqrt{`%d`}}`%c%d`}{`2`}}\t", 
			B, (A>=0?'+':'-'), (int)fabs(A), B, (A>=0?'+':'-'), (int)fabs(A)); pos += bytes;
		bytes = sprintf(answers + pos,  "{\\dfrac{`-`{\\sqrt{`%d`}}`%c%d`}{`2`}},{\\dfrac{`+`{\\sqrt{`%d`}}`%c%d`}{`2`}}",
			B, (A>=0?'+':'-'), (int)fabs(A), B, (A>=0?'+':'-'), (int)fabs(A)); pos += bytes;
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_1_29(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 8 + 2;
			B = rand() % 18 + 2;
			C = rand() % 18 + 2;
		}	
		while (isprime(B) == false || isprime(C) == false || B == C || fabs(A*B+C) <= 1.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve the equation using any method.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(%dx^{2}+%d = %dx,\\ =\\ \\ ?\\)</p>", 
			A, B*C, A*B+C); pos += bytes;
		bytes = sprintf(content + pos, "<p>Separate answers with a comma.</p>"); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "{\\dfrac{`%d`}{`%d`}},%d\t", C, A, B); pos += bytes;
		bytes = sprintf(answers + pos,  "%d,{\\dfrac{`%d`}{`%d`}}", B, C, A); pos += bytes;
	
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_1_30(const char *f)
{
		if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 8 + 2;
			B = rand() % 18 + 2;
		}	
		while (isprime(A) == false || isprime(B) == false || A == B);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve the equation using any method.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(%dx^{2}-%d=0,\\ =\\ \\ ?\\)</p>", 
			(int)pow(A,2), (int)pow(B,2)); pos += bytes;
		bytes = sprintf(content + pos, "<p>Separate answers with a comma.</p>"); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "{\\pm}{\\dfrac{`%d`}{`%d`}}\t", B, A); pos += bytes;
		bytes = sprintf(answers + pos,  "{\\dfrac{`%d`}{`%d`}},-{\\dfrac{`%d`}{`%d`}}\t", B, A, B, A); pos += bytes;
		bytes = sprintf(answers + pos,  "-{\\dfrac{`%d`}{`%d`}},{\\dfrac{`%d`}{`%d`}}\t", B, A, B, A); pos += bytes;
		bytes = sprintf(answers + pos,  "{\\dfrac{`%d`}{`%d`}},{\\dfrac{`-%d`}{`%d`}}\t", B, A, B, A); pos += bytes;
		bytes = sprintf(answers + pos,  "{\\dfrac{`-%d`}{`%d`}},{\\dfrac{`%d`}{`%d`}}\t", B, A, B, A); pos += bytes;
		bytes = sprintf(answers + pos,  "{\\dfrac{`%d`}{`%d`}},{\\dfrac{`%d`}{`-%d`}}\t", B, A, B, A); pos += bytes;
		bytes = sprintf(answers + pos,  "{\\dfrac{`%d`}{`-%d`}},{\\dfrac{`%d`}{`%d`}}", B, A, B, A); pos += bytes;
	
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};


int question_2_01(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 19 - 9;
			B = rand() % 19 - 9;
			C = rand() % 19 - 9;
		}
		while (fabs(A) <= 1.0f || fabs(B) <= 1.0f || C == 0 || fabs(C*C+A*C+B) >= 100.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Evaluate the function \\(f(x) = x^{2}%c%dx%c%d\\) at the given value.</p>", 
			(A>=0?'+':'-'), (int)fabs(A), (B>=0?'+':'-'), (int)fabs(B)); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(f(%d) =\\ \\ ?\\)</p>", C); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos, "%d", C*C+A*C+B); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_2_02(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 19 - 9;
			B = rand() % 19 - 9;
			C = rand() % 19 - 9;
		}
		while (fabs(A) <= 1.0f || fabs(B) <= 1.0f || C == 0 || fabs(2*C+A) <= 1.0f || fabs(C*C+A*C+B) <= 1.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Evaluate the function \\(f(x) = x^{2}%c%dx%c%d\\) at the given value.</p>", 
			(A>=0?'+':'-'), (int)fabs(A), (B>=0?'+':'-'), (int)fabs(B)); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(f(x%c%d) =\\ \\ ?\\)</p>", (C>=0?'+':'-'), (int)fabs(C)); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos, "x{^{`2`}}%c%dx%c%d\t", (2*C+A>=0?'+':'-'), (int)fabs(2*C+A), (C*C+A*C+B>=0?'+':'-'), (int)fabs(C*C+A*C+B)); pos += bytes;
		bytes = sprintf(answers + pos, "%dx+x{^{`2`}}%c%d\t", 2*C+A, (C*C+A*C+B>=0?'+':'-'), (int)fabs(C*C+A*C+B)); pos += bytes;
		bytes = sprintf(answers + pos, "%dx%c%d+x{^{`2`}}\t", 2*C+A, (C*C+A*C+B>=0?'+':'-'), (int)fabs(C*C+A*C+B)); pos += bytes;
		bytes = sprintf(answers + pos, "x{^{`2`}}%c%d%c%dx\t", (C*C+A*C+B>=0?'+':'-'), (int)fabs(C*C+A*C+B), (2*C+A>=0?'+':'-'), (int)fabs(2*C+A)); pos += bytes;
		bytes = sprintf(answers + pos, "%d+x{^{`2`}}%c%dx\t", C*C+A*C+B, (2*C+A>=0?'+':'-'), (int)fabs(2*C+A)); pos += bytes;
		bytes = sprintf(answers + pos, "%d%c%dx+x{^{`2`}}", C*C+A*C+B, (2*C+A>=0?'+':'-'), (int)fabs(2*C+A)); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_2_03(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = -(rand() % 9 + 1);
			B = rand() % 19 - 9;
			C = rand() % 9 + 1;
			D = rand() % 19 - 9;
		}
		while (A == C || B == D || B == 0 || D == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		if (randomizer% 2 == 0)
		{
			bytes = sprintf(content + pos, "<p>Use the graph of \\(g\\) to find \\(g(%d)\\).</p>", A); pos += bytes;
			bytes = sprintf(content + pos, "@f(x) = x<%d ? %f*x+%f : x>%d ? %f*x+%f : %f*x+%f ; ",
				A, (float)(B-0)/(float)(A+15), -(float)(B-0)/(float)(A+15)*A+B,
				C, (float)(D-0)/(float)(C-15), -(float)(D-0)/(float)(C-15)*C+D,
				(float)(D-B)/(float)(C-A), -(float)(D-B)/(float)(C-A)*A+B); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos,  "<p>\\(g(%d) =\\ \\ ?\\)</p>", A); pos += bytes;

			bytes = 0;
			pos = 0;			

			bytes = sprintf(answers + pos, "%d", B); pos += bytes;
		}
		else
		{
			bytes = sprintf(content + pos, "<p>Use the graph of \\(g\\) to find \\(g(%d)\\).</p>", C); pos += bytes;
			bytes = sprintf(content + pos, "@f(x) = x<%d ? %f*x+%f : x>%d ? %f*x+%f : %f*x+%f ; ",
				A, (float)(B-0)/(float)(A+15), -(float)(B-0)/(float)(A+15)*A+B,
				C, (float)(D-0)/(float)(C-15), -(float)(D-0)/(float)(C-15)*C+D,
				(float)(D-B)/(float)(C-A), -(float)(D-B)/(float)(C-A)*A+B); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos,  "<p>\\(g(%d) =\\ \\ ?\\)</p>", C); pos += bytes;

			bytes = 0;
			pos = 0;			

			bytes = sprintf(answers + pos, "%d", D); pos += bytes;
		}

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_2_04(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		A = rand() % 11 - 5;
		B = rand() % 11 - 5;

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		if (randomizer% 2 == 0)
		{
			bytes = sprintf(content + pos, "<p>Use the graph to determine the function's domain.</p>"); pos += bytes;
			bytes = sprintf(content + pos, "@f(x) = sqrt(x-%d)+%d ; set object 1 circle at %d,%d size 0.333 fillcolor rgb 'dark-violet' fill solid noborder; ", A, B, A, B); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos, "<p>The domain of the function in interval notation is \\(=\\ \\ ?\\)</p>"); pos += bytes;

			bytes = 0;
			pos = 0;			

			bytes = sprintf(answers + pos, "[%d,{\\infty})", A); pos += bytes;
		}
		else
		{
			bytes = sprintf(content + pos, "<p>Use the graph to determine the function's range.</p>"); pos += bytes;
			bytes = sprintf(content + pos, "@f(x) = sqrt(x-%d)+%d ; set object 1 circle at %d,%d size 0.333 fillcolor rgb 'dark-violet' fill solid noborder; ", A, B, A, B); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos, "<p>The range of the function in interval notation is \\(=\\ \\ ?\\)</p>"); pos += bytes;

			bytes = 0;
			pos = 0;			

			bytes = sprintf(answers + pos, "[%d,{\\infty})", B); pos += bytes;
		}

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_2_05(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		A = rand() % 11 - 5;
		B = rand() % 11 - 5;

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Use the graph of to determine the open interval on which the function is decreasing.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "@f(x) = x<%d ? -x+%d : x>%d ? %d : %d ; ",
			A, A+B, A, B, B); pos += bytes;
		bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
		bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
		bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
		bytes = sprintf(content + pos,  "<p>The function is decreasing on the interval \\(=\\ \\ ?\\)</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos, "(-{\\infty},%d)", A); pos += bytes;
	
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_2_06(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		A = rand() % 11 - 5;
		B = rand() % 11 - 5;

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Use the graph to determine the function's relative minimum.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "@f(x) = (x-%d)**2+%d ;", A, B); pos += bytes;
		bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
		bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
		bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
		bytes = sprintf(content + pos, "<p>The relative minimum of the function in (x,y) point form is \\(=\\ \\ ?\\)</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "(%d,%d)", A, B); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_2_07(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		A = rand() % 3;
		B = rand() % 3;

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Use possible symmetry to determine whether the graph is the graph of an even function, odd function, or neither.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "@f(x) = (x-%d)**2*(x-%d)**2/%d ; ", 
			-1-A, 1+B, 3*(A+B+1)); pos += bytes;
		bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
		bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
		bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
		bytes = sprintf(content + pos, "<p>The function is:</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>a) Even</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>b) Odd</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>c) Neither</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		if (A == B)
		{
			bytes = sprintf(answers + pos, "{a}"); pos += bytes;
		}
		else
		{
			bytes = sprintf(answers + pos, "{c}"); pos += bytes;
		}

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_2_08(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 5 + 5;
			B = rand() % 2 + 3;
			C = rand() % 19 - 9;
		}
		while (fabs(C) <= 1.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Determine whether the function is even, odd, or neither.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(f(x) = x^{%d}%c%dx^{%d}\\)</p>", A, (C>0?'+':'-'), (int)fabs(C), B); pos += bytes;
		bytes = sprintf(content + pos, "<p>The function is:</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>a) Even</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>b) Odd</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>c) Neither</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		if (A % 2 == 0 && B % 2 == 0)
		{
			bytes = sprintf(answers + pos, "{a}"); pos += bytes;
		}
		else if (A % 2 == 1 && B % 2 == 1)
		{
			bytes = sprintf(answers + pos, "{b}"); pos += bytes;
		}
		else
		{
			bytes = sprintf(answers + pos, "{c}"); pos += bytes;
		}

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_2_09(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		A = rand() % 4 + 2;
		B = rand() % 2;

		if (B == 0) B = -1;

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Pick the piecewise function representing this graph.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "@f(x) = x<0 ? %d*x : x>0 ? %d*x : %d*x ;",
			-A*B, A*B, A*B); pos += bytes;
		bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
		bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
		bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
		bytes = sprintf(content + pos, "<p>The function is:</p>"); pos += bytes;

		if (randomizer% 6 == 0)
		{
			bytes = sprintf(content + pos, "<p>a) \\(f(x) = \\begin{cases}%dx&,\\ x<0\\\\%dx&,\\ x\\ge 0\\end{cases}\\)</p>", -A*B, A*B);  pos += bytes;
			bytes = sprintf(content + pos, "<p>b) \\(f(x) = \\begin{cases}%dx&,\\ x<0\\\\%dx&,\\ x\\ge 0\\end{cases}\\)</p>", A*B, -A*B);  pos += bytes;
			bytes = sprintf(content + pos, "<p>c) \\(f(x) = \\begin{cases}%dx&,\\ x<0\\\\%dx&,\\ x\\ge 0\\end{cases}\\)</p>", -A*B, -A*B);  pos += bytes;
			bytes = sprintf(content + pos, "<p>d) \\(f(x) = \\begin{cases}%dx&,\\ x<0\\\\%dx&,\\ x\\ge 0\\end{cases}\\)</p>", A*B, A*B);  pos += bytes;

			bytes = 0;
			pos = 0;			

			bytes = sprintf(answers + pos, "{a}"); pos += bytes;
		}
		else if (randomizer% 6 == 1)
		{
			bytes = sprintf(content + pos, "<p>a) \\(f(x) = \\begin{cases}%dx&,\\ x<0\\\\%dx&,\\ x\\ge 0\\end{cases}\\)</p>", A*B, -A*B);  pos += bytes;
			bytes = sprintf(content + pos, "<p>b) \\(f(x) = \\begin{cases}%dx&,\\ x<0\\\\%dx&,\\ x\\ge 0\\end{cases}\\)</p>", -A*B, A*B);  pos += bytes;
			bytes = sprintf(content + pos, "<p>c) \\(f(x) = \\begin{cases}%dx&,\\ x<0\\\\%dx&,\\ x\\ge 0\\end{cases}\\)</p>", -A*B, -A*B);  pos += bytes;
			bytes = sprintf(content + pos, "<p>d) \\(f(x) = \\begin{cases}%dx&,\\ x<0\\\\%dx&,\\ x\\ge 0\\end{cases}\\)</p>", A*B, A*B);  pos += bytes;

			bytes = 0;
			pos = 0;			

			bytes = sprintf(answers + pos, "{b}"); pos += bytes;
		}
		else if (randomizer% 6 == 2)
		{
			bytes = sprintf(content + pos, "<p>a) \\(f(x) = \\begin{cases}%dx&,\\ x<0\\\\%dx&,\\ x\\ge 0\\end{cases}\\)</p>", A*B, -A*B);  pos += bytes;
			bytes = sprintf(content + pos, "<p>b) \\(f(x) = \\begin{cases}%dx&,\\ x<0\\\\%dx&,\\ x\\ge 0\\end{cases}\\)</p>", -A*B, -A*B);  pos += bytes;
			bytes = sprintf(content + pos, "<p>c) \\(f(x) = \\begin{cases}%dx&,\\ x<0\\\\%dx&,\\ x\\ge 0\\end{cases}\\)</p>", -A*B, A*B);  pos += bytes;
			bytes = sprintf(content + pos, "<p>d) \\(f(x) = \\begin{cases}%dx&,\\ x<0\\\\%dx&,\\ x\\ge 0\\end{cases}\\)</p>", A*B, A*B);  pos += bytes;

			bytes = 0;
			pos = 0;			

			bytes = sprintf(answers + pos, "{c}"); pos += bytes;
		}
		else if (randomizer% 6 == 3)
		{
			bytes = sprintf(content + pos, "<p>a) \\(f(x) = \\begin{cases}%dx&,\\ x<0\\\\%dx&,\\ x\\ge 0\\end{cases}\\)</p>", -A*B, A*B);  pos += bytes;
			bytes = sprintf(content + pos, "<p>b) \\(f(x) = \\begin{cases}%dx&,\\ x<0\\\\%dx&,\\ x\\ge 0\\end{cases}\\)</p>", -A*B, -A*B);  pos += bytes;
			bytes = sprintf(content + pos, "<p>c) \\(f(x) = \\begin{cases}%dx&,\\ x<0\\\\%dx&,\\ x\\ge 0\\end{cases}\\)</p>", A*B, A*B);  pos += bytes;
			bytes = sprintf(content + pos, "<p>d) \\(f(x) = \\begin{cases}%dx&,\\ x<0\\\\%dx&,\\ x\\ge 0\\end{cases}\\)</p>", A*B, -A*B);  pos += bytes;

			bytes = 0;
			pos = 0;			

			bytes = sprintf(answers + pos, "{a}"); pos += bytes;
		}
		else if (randomizer% 6 == 4)
		{
			bytes = sprintf(content + pos, "<p>a) \\(f(x) = \\begin{cases}%dx&,\\ x<0\\\\%dx&,\\ x\\ge 0\\end{cases}\\)</p>", -A*B, -A*B);  pos += bytes;
			bytes = sprintf(content + pos, "<p>b) \\(f(x) = \\begin{cases}%dx&,\\ x<0\\\\%dx&,\\ x\\ge 0\\end{cases}\\)</p>", A*B, A*B);  pos += bytes;
			bytes = sprintf(content + pos, "<p>c) \\(f(x) = \\begin{cases}%dx&,\\ x<0\\\\%dx&,\\ x\\ge 0\\end{cases}\\)</p>", -A*B, A*B);  pos += bytes;
			bytes = sprintf(content + pos, "<p>d) \\(f(x) = \\begin{cases}%dx&,\\ x<0\\\\%dx&,\\ x\\ge 0\\end{cases}\\)</p>", A*B, -A*B);  pos += bytes;			

			bytes = 0;
			pos = 0;			

			bytes = sprintf(answers + pos, "{c}"); pos += bytes;
		}
		else if (randomizer% 6 == 5)
		{
			bytes = sprintf(content + pos, "<p>a) \\(f(x) = \\begin{cases}%dx&,\\ x<0\\\\%dx&,\\ x\\ge 0\\end{cases}\\)</p>", -A*B, -A*B);  pos += bytes;
			bytes = sprintf(content + pos, "<p>b) \\(f(x) = \\begin{cases}%dx&,\\ x<0\\\\%dx&,\\ x\\ge 0\\end{cases}\\)</p>", A*B, A*B);  pos += bytes;
			bytes = sprintf(content + pos, "<p>c) \\(f(x) = \\begin{cases}%dx&,\\ x<0\\\\%dx&,\\ x\\ge 0\\end{cases}\\)</p>", A*B, -A*B);  pos += bytes;
			bytes = sprintf(content + pos, "<p>d) \\(f(x) = \\begin{cases}%dx&,\\ x<0\\\\%dx&,\\ x\\ge 0\\end{cases}\\)</p>", -A*B, A*B);  pos += bytes;

			bytes = 0;
			pos = 0;			

			bytes = sprintf(answers + pos, "{d}"); pos += bytes;
		}

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;
	return 1;
};

int question_2_10(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 8 + 2;
			B = rand() % 8 + 2;
			C = rand() % 19 - 9;
			D = rand() % 19 - 9;
		}
		while (A == 0 || B == 0 || C == 0 || D == 0 || !isprime(A) || !isprime(B) || A == B || !isinteger((float)(-A*C)/(float)(B)+(float)D) ||
			(int)fabs(-(A*C)/B+D) <= 1.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Use the given conditions to write an equation for the line.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>Slope: \\(\\dfrac{ %d }{ %d }\\), Point: \\((%d,%d)\\)</p>",
			A, B, C, D); pos += bytes;
		bytes = sprintf(content + pos, "<p>Write the equation of the line in slope-intercept \\(y=mx+b\\) form.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "y={\\dfrac{`%d`}{`%d`}}x%c%d", A, B, (-(A*C)/B+D>0?'+':'-'), (int)fabs(-(A*C)/B+D)) ; pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_2_11(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 19 - 9;
			B = rand() % 19 - 9;
			C = rand() % 19 - 9;
			D = rand() % 19 - 9;
		}
		while (A == 0 || B == 0 || C == 0 || D == 0 || A == C || B == D || 
			!isprime((int)fabs(C-A)) || !isprime((int)fabs(D-B)) || (int)fabs(C-A) == (int)fabs(D-B) || (float)(D-B)/(float)(C-A) <= 0.0f || 
			(int)fabs(C-A) <= 1 || (int)fabs(D-B) <= 1 || !isinteger(-(float)(D-B)/(float)(C-A)*(float)A+(float)B) ||
			fabs(-(float)(D-B)/(float)(C-A)*(float)A+(float)B) <= 1.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Use the given conditions to write an equation for the line.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>Points: \\((%d,%d)\\), and \\((%d,%d)\\)</p>",
			A, B, C, D); pos += bytes;
		bytes = sprintf(content + pos, "<p>Write the equation of the line in slope-intercept \\(y=mx+b\\) form.  Use fractions when necessary.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "y={\\dfrac{`%d`}{`%d`}}x%c%d", (int)fabs(D-B), (int)fabs(C-A), 
			(-(float)(D-B)/(float)(C-A)*(float)A+(float)B>0?'+':'-'), (int)fabs(-(float)(D-B)/(float)(C-A)*(float)A+(float)B)); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_2_12(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 11 - 5;
			B = rand() % 11 - 5;
		}
		while (fabs(A) <= 1.0f || fabs(B) <= 1.0f || !isprime((int)fabs(A)) || !isprime((int)fabs(B)) || A == B || (float)B/(float)A <= 0.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Use the graph to find the equation of the line.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "@f(x) = %d*x/%d+%d ;",
			B, A, B); pos += bytes;
		bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
		bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
		bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
		bytes = sprintf(content + pos, "<p>Write the equation of the line in slope-intercept \\(y=mx+b\\) form.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "y={\\dfrac{`%d`}{`%d`}}x%c%d", (int)fabs(B), (int)fabs(A), (B>0?'+':'-'), (int)fabs(B)); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_2_13(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 4 + 5;
			B = rand() % 4 + 5;
			C = rand() % 4 + 1;
		}
		while (isinteger((float)(C-A)/(float)(B-0)));

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Use the points on the graph to find the equation of the line.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "@f(x) = %f*x/%f+%d ; set object 1 circle at %d,%d size 0.333 fillcolor rgb 'dark-violet' fill solid noborder ;",
			(float)(C-A), (float)(B-0), A, 0, A); pos += bytes;
		bytes = sprintf(content + pos, "set object 2 circle at %d,%d size 0.333 fillcolor rgb 'dark-violet' fill solid noborder ;", B, C); pos += bytes;
		bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
		bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
		bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
		bytes = sprintf(content + pos, "<p>Write the equation of the line in slope-intercept \\(y=mx+b\\) form.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>Round to one decimal place.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "y=%.1fx+%d\t", (float)(C-A)/(float)(B-0), A); pos += bytes;
		bytes = sprintf(answers + pos,  "y=%.1fx+%d.0\t", (float)(C-A)/(float)(B-0), A); pos += bytes;
		if ((int)((float)(C-A)/(float)(B-0)*10.0f) > 0)
		{
			bytes = sprintf(answers + pos,  "y=.%dx+%d\t", (int)((float)(C-A)/(float)(B-0)*10.0f), A); pos += bytes;
			bytes = sprintf(answers + pos,  "y=.%dx+%d.0", (int)((float)(C-A)/(float)(B-0)*10.0f), A); pos += bytes;
		}
		else
		{
			bytes = sprintf(answers + pos,  "y=-.%dx+%d\t", (int)fabs((float)(C-A)/(float)(B-0)*10.0f), A); pos += bytes;
			bytes = sprintf(answers + pos,  "y=-.%dx+%d.0", (int)fabs((float)(C-A)/(float)(B-0)*10.0f), A); pos += bytes;
		}

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_2_14(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 11 - 5;
			B = rand() % 11 - 5;
		}
		while (A == 0 || B == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Find the function from the graph using transformations.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "@f(x) = (x-%d)**2+%d ; ",
			A, B); pos += bytes;
		bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
		bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
		bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
		bytes = sprintf(content + pos, "<p>Write the equation of the parabola in the \\(y=\\pm a(x\\pm x_{0})^{2}\\pm y_{0}\\) form.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "y=(x%c%d){^{`2`}}%c%d\t", (A>0?'-':'+'), (int)fabs(A), (B>0?'+':'-'), (int)fabs(B)); pos += bytes;
		bytes = sprintf(answers + pos,  "y=+(x%c%d){^{`2`}}%c%d\t", (A>0?'-':'+'), (int)fabs(A), (B>0?'+':'-'), (int)fabs(B)); pos += bytes;
		bytes = sprintf(answers + pos,  "y=1(x%c%d){^{`2`}}%c%d\t", (A>0?'-':'+'), (int)fabs(A), (B>0?'+':'-'), (int)fabs(B)); pos += bytes;
		bytes = sprintf(answers + pos,  "y=+1(x%c%d){^{`2`}}%c%d", (A>0?'-':'+'), (int)fabs(A), (B>0?'+':'-'), (int)fabs(B)); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_2_15(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 11 - 5;
		}
		while (A == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Find the function from the graph using transformations.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "@f(x) = -(x-%d)**2 ; ",
			A); pos += bytes;
		bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
		bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
		bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
		bytes = sprintf(content + pos, "<p>Write the equation of the parabola in the \\(y=\\pm a(x\\pm x_{0})^{2}\\pm y_{0}\\) form.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "y=-(x%c%d){^{`2`}}\t", (A>0?'-':'+'), (int)fabs(A)); pos += bytes;
		bytes = sprintf(answers + pos,  "y=-(x%c%d){^{`2`}}+0\t", (A>0?'-':'+'), (int)fabs(A)); pos += bytes;
		bytes = sprintf(answers + pos,  "y=-(x%c%d){^{`2`}}-0\t", (A>0?'-':'+'), (int)fabs(A)); pos += bytes;
		bytes = sprintf(answers + pos,  "y=-1(x%c%d){^{`2`}}\t", (A>0?'-':'+'), (int)fabs(A)); pos += bytes;
		bytes = sprintf(answers + pos,  "y=-1(x%c%d){^{`2`}}+0\t", (A>0?'-':'+'), (int)fabs(A)); pos += bytes;
		bytes = sprintf(answers + pos,  "y=-1(x%c%d){^{`2`}}-0", (A>0?'-':'+'), (int)fabs(A)); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;
	
	return 1;
};

int question_2_16(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 11 - 5;
			B = rand() % 11 - 5;
		}
		while (A == 0 || B == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Find the function from the graph using transformations.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "@f(x) = -(x-%d)**2+%d ; ",
			A, B); pos += bytes;
		bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
		bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
		bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
		bytes = sprintf(content + pos, "<p>Write the equation of the parabola in the \\(y=\\pm a(x\\pm x_{0})^{2}\\pm y_{0}\\) form.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "y=-(x%c%d){^{`2`}}%c%d\t", (A>0?'-':'+'), (int)fabs(A), (B>0?'+':'-'), (int)fabs(B)); pos += bytes;
		bytes = sprintf(answers + pos,  "y=-1(x%c%d){^{`2`}}%c%d", (A>0?'-':'+'), (int)fabs(A), (B>0?'+':'-'), (int)fabs(B)); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;
	
	return 1;
};

int question_2_17(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 2 + 2;
			B = rand() % 11 - 5;
			C = rand() % 11 - 5;
		}
		while (B == 0 || C == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Find the function from the graph using transformations.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "@f(x) = %d*(x-%d)**2+%d ; ",
			A, B, C); pos += bytes;
		bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
		bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
		bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
		bytes = sprintf(content + pos, "<p>Write the equation of the parabola in the \\(y=\\pm a(x\\pm x_{0})^{2}\\pm y_{0}\\) form.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "y=%d(x%c%d){^{`2`}}%c%d\t", A, (B>0?'-':'+'), (int)fabs(B), (C>0?'+':'-'), (int)fabs(C)); pos += bytes;
		bytes = sprintf(answers + pos,  "y=+%d(x%c%d){^{`2`}}%c%d", A, (B>0?'-':'+'), (int)fabs(B), (C>0?'+':'-'), (int)fabs(C)); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;
	
	return 1;
};

int question_2_18(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 19 - 9;
			B = -(rand() % 10) - 1;
			C = rand() % 10 + 1;
		}
		while (A == 0 || fabs(-B-C) <= 1.0f || B*C == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Find the domain of the function.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(f(x) = \\dfrac{ %d }{ x^{2}%c%dx%c%d }\\)</p>",
			A, (-B-C>0?'+':'-'), (int)fabs(-B-C), (B*C>0?'+':'-'), (int)fabs(B*C)); pos += bytes;
		bytes = sprintf(content + pos, "<p>The domain of the function in interval notation is \\(=\\ \\ ?\\)</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "(-{\\infty},%d){\\cup}(%d,%d){\\cup}(%d,{\\infty})", B, B, C, C); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_2_19(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 8 + 2;
			B = rand() % 11 - 5;
		}
		while (A == 0 || B == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Find the domain of the function.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(f(x) = \\sqrt{%dx%c%d}\\)</p>",
			A, (A*B>0?'-':'+'), (int)fabs(A*B)); pos += bytes;
		bytes = sprintf(content + pos, "<p>The domain of the function in interval notation is \\(=\\ \\ ?\\)</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "[%d,{\\infty})", B); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_2_20(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 8 + 2;
			B = rand() % 11 - 5;
			C = rand() % 11 - 5;
		}
		while (fabs(A) <= 1.0f || B == 0 || C == 0 || B+C == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Find the indicated combination given the functions below.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(f(x) = %dx%c%d\\) and \\(g(x) = x%c%d\\)</p>",
			A, (B>0?'+':'-'), (int)fabs(B), (C>0?'+':'-'), (int)fabs(C)); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\((f+g)(x) =\\ \\ ?\\)</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>Simplify completely.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%dx%c%d\t", A+1, (B+C>0?'+':'-'), (int)fabs(B+C)); pos += bytes;
		bytes = sprintf(answers + pos,  "%d%c%dx", B+C, (A+1>0?'+':'-'), (int)fabs(A+1)); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_2_21(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 7 + 3;
			B = rand() % 11 - 5;
			C = rand() % 11 - 5;
		}
		while (fabs(A) <= 1.0f || B == 0 || C == 0 || fabs(A-1) <= 1.0f || B-C == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Find the indicated combination given the functions below.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(f(x) = %dx%c%d\\) and \\(g(x) = x%c%d\\)</p>",
			A, (B>0?'+':'-'), (int)fabs(B), (C>0?'+':'-'), (int)fabs(C)); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\((f-g)(x) =\\ \\ ?\\)</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>Simplify completely.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%dx%c%d\t", A-1, (B-C>0?'+':'-'), (int)fabs(B-C)); pos += bytes;
		bytes = sprintf(answers + pos,  "%d%c%dx", B-C, (A-1>0?'+':'-'), (int)fabs(A-1)); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_2_22(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 8 + 2;
			B = rand() % 11 - 5;
			C = rand() % 11 - 5;
		}
		while (fabs(A) <= 1.0f || B == 0 || C == 0 || fabs(A*C+B) <= 1.0f || B*C == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Find the indicated combination given the functions below.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(f(x) = %dx%c%d\\) and \\(g(x) = x%c%d\\)</p>",
			A, (B>0?'+':'-'), (int)fabs(B), (C>0?'+':'-'), (int)fabs(C)); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\((fg)(x) =\\ \\ ?\\)</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>Expand and simplify if needed.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%dx{^{`2`}}%c%dx%c%d\t", A, (A*C+B>0?'+':'-'), (int)fabs(A*C+B), (B*C>0?'+':'-'), (int)fabs(B*C)); pos += bytes;
		bytes = sprintf(answers + pos,  "%dx+%dx{^{`2`}}%c%d\t", A*C+B, A, (B*C>0?'+':'-'), (int)fabs(B*C)); pos += bytes;
		bytes = sprintf(answers + pos,  "%dx%c%d+%dx{^{`2`}}\t", A*C+B, (B*C>0?'+':'-'), (int)fabs(B*C), A); pos += bytes;
		bytes = sprintf(answers + pos,  "%dx{^{`2`}}%c%d%c%dx\t", A, (B*C>0?'+':'-'), (int)fabs(B*C), (A*C+B>0?'+':'-'), (int)fabs(A*C+B)); pos += bytes;
		bytes = sprintf(answers + pos,  "%d+%dx{^{`2`}}%c%dx\t", B*C, A, (A*C+B>0?'+':'-'), (int)fabs(A*C+B)); pos += bytes;
		bytes = sprintf(answers + pos,  "%d%c%dx+%dx{^{`2`}}", B*C, (A*C+B>0?'+':'-'), (int)fabs(A*C+B), A); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_2_23(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 8 + 2;
			B = rand() % 11 - 5;
			C = rand() % 11 - 5;
		}
		while (fabs(A) <= 1.0f || !isprime(A) || B == 0 || C == 0 || !isprime((int)fabs(B)) || A == B || A == -B);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Find the indicated combination given the functions below.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(f(x) = %dx%c%d\\) and \\(g(x) = x%c%d\\)</p>",
			A, (B>0?'+':'-'), (int)fabs(B), (C>0?'+':'-'), (int)fabs(C)); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\((\\dfrac{ f }{ g })(x) =\\ \\ ?\\)</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>Expand and simplify if needed.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "{\\dfrac{`%dx%c%d`}{`x%c%d`}}\t", A, (B>0?'+':'-'), (int)fabs(B), (C>0?'+':'-'), (int)fabs(C)); pos += bytes;
		bytes = sprintf(answers + pos,  "{\\dfrac{`%d+%dx`}{`x%c%d`}}\t", B, A, (C>0?'+':'-'), (int)fabs(C)); pos += bytes;
		bytes = sprintf(answers + pos,  "{\\dfrac{`%dx%c%d`}{`%d+x`}}\t", A, (B>0?'+':'-'), (int)fabs(B), C); pos += bytes;
		bytes = sprintf(answers + pos,  "{\\dfrac{`%d+%dx`}{`%d+x`}}", B, A, C); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_2_24(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 8 + 2;
			B = rand() % 11 - 5;
			C = rand() % 3 + 2;
			D = rand() % 11 - 5;
		}
		while (B == 0 || D == 0 || fabs(A*C) <= 1.0f || A*D+B == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Find the indicated composition given the functions below.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(f(x) = %dx%c%d\\) and \\(g(x) = %dx%c%d\\)</p>",
			A, (B>0?'+':'-'), (int)fabs(B), C, (D>0?'+':'-'), (int)fabs(D)); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\((f\\circ g)(x) =\\ \\ ?\\)</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>Expand and simplify completely.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%dx%c%d\t", A*C, (A*D+B>0?'+':'-'), (int)fabs(A*D+B)); pos += bytes;
		bytes = sprintf(answers + pos,  "%d+%dx", A*D+B, A*C); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_2_25(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 2 + 2;
			B = rand() % 41 - 20;
		}
		while (A == 0 || B == 0 || (int)fabs(B) % A == 0 || isinteger(sqrt(A)) || isinteger(sqrt((int)fabs(B))) );

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Find the indicated composition given the functions below.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(f(x) = \\sqrt{x}\\) and \\(g(x) = %dx^{2}%c%d\\)</p>",
			A, (B>0?'+':'-'), (int)fabs(B)); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\((f\\circ g)(x) =\\ \\ ?\\)</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>Expand and simplify completely.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "{\\sqrt{`%dx`{^{`2`}}`%c%d`}}\t", A, (B>0?'+':'-'), (int)fabs(B)); pos += bytes;
		bytes = sprintf(answers + pos,  "{\\sqrt{`%d+%dx`{^{`2`}}}}", B, A); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;	

	return 1;
};

int question_2_26(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 19 - 9;
		}
		while (A == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Find the inverse function.</p>"); pos += bytes;

		if (randomizer % 2 == 0)
		{
			bytes = sprintf(content + pos, "<p>\\(f(x) = x^{3}%c%d\\)</p>",
				(A>0?'+':'-'), (int)fabs(A)); pos += bytes;
			bytes = sprintf(content + pos, "<p>\\(f^{-1}(x) =\\ \\ ?\\)</p>"); pos += bytes;
	
			bytes = 0;
			pos = 0;			
	
			bytes = sprintf(answers + pos,  "{\\sqrt[3]{`x%c%d`}}", (A>0?'-':'+'), (int)fabs(A)); pos += bytes;
		}
		else
		{
			bytes = sprintf(content + pos, "<p>\\(f(x) = (x%c%d)^{3}\\)</p>",
				(A>0?'+':'-'), (int)fabs(A)); pos += bytes;
			bytes = sprintf(content + pos, "<p>\\(f^{-1}(x) =\\ \\ ?\\)</p>"); pos += bytes;
	
			bytes = 0;
			pos = 0;			
	
			bytes = sprintf(answers + pos,  "{\\sqrt[3]{`x`}}%c%d", (A>0?'-':'+'), (int)fabs(A)); pos += bytes;
		}

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;	


	return 1;
};

int question_2_27(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		A = rand() % 11 - 5;
		B = rand() % 11 - 5;
		C = rand() % 2 + 2;

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Determine if the graph represents a function that has an inverse.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "@f(x) = (x-%d)**%d+%d ; ",
			A, C, B); pos += bytes;
		bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
		bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
		bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
		bytes = sprintf(content + pos, "<p>Does the function have an inverse?</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>a) Yes</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>b) No</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		if (C % 2 == 1)
		{
			bytes = sprintf(answers + pos,  "{a}"); pos += bytes;
		}
		else
		{
			bytes = sprintf(answers + pos,  "{b}"); pos += bytes;
		}

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_2_28(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 11 - 5;
			B = rand() % 11 - 5;
			C = rand() % 5 - 2;
		} while (C == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Determine if the graph represents a function that has an inverse.</p>"); pos += bytes;
		
		if (randomizer % 2 == 0)
		{
			bytes = sprintf(content + pos, "@f(x) = %d*(x-%d)+%d ; ",
				C, A, B); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos, "<p>Does the function have an inverse?</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>a) Yes</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>b) No</p>"); pos += bytes;
	
			bytes = 0;
			pos = 0;			
	
			bytes = sprintf(answers + pos,  "{a}"); pos += bytes;
		}
		else
		{
			bytes = sprintf(content + pos, "@f(x) = %d*sin(x-%d)+%d ; ",
				C, A, B); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos, "<p>Does the function have an inverse?</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>a) Yes</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>b) No</p>"); pos += bytes;
	
			bytes = 0;
			pos = 0;			
	
			bytes = sprintf(answers + pos,  "{b}"); pos += bytes;
		}

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_2_29(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 11 - 5;
			B = rand() % 11 - 5;
			C = rand() % 11 - 5;
			D = rand() % 11 - 5;
		} while (fabs(A - C) <= 2.0f || fabs(B - D) <= 2.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Use the graph below to answer the question.</p>"); pos += bytes;
		
		if (randomizer% 4 == 0)
		{
			bytes = sprintf(content + pos, "@f(x) = (x-%d)**3+%d ; cuberoot(x) = sgn(x)*abs(x)**(1/3.0) ; g(x) = cuberoot(x-%d)+%d  ; ",
				A, B, B, A); pos += bytes;
		}
		else if (randomizer% 4 == 1)
		{
			bytes = sprintf(content + pos, "@f(x) = (x-%d)**3+%d ; cuberoot(x) = sgn(x)*abs(x)**(1/3.0) ; g(x) = cuberoot(x-%d)+%d  ; ",
				A, B, D, C); pos += bytes;
		}
		else if (randomizer% 4 == 2)
		{
			bytes = sprintf(content + pos, "@f(x) = 2**(x-%d)+%d ; logarithm(x) = log(x)/log(2) ; g(x) = x > %f ? logarithm(x-%d)+%d : -15 ; ",
				A, B, (float)(B+0.25f), B, A); pos += bytes;
		}
		else
		{
			bytes = sprintf(content + pos, "@f(x) = 2**(x-%d)+%d ; logarithm(x) = log(x)/log(2) ; g(x) = x > %f ? logarithm(x-%d)+%d : -15 ; ",
				A, B, (float)(D+0.25f), D, C); pos += bytes;
		}

		bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
		bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
		bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
		bytes = sprintf(content + pos, "<p>Does the graph show both the function and its inverse?</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>a) Yes</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>b) No</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		if (randomizer% 4 == 0)
		{
			bytes = sprintf(answers + pos,  "{a}"); pos += bytes;
		}
		else if (randomizer% 4 == 1)
		{
			bytes = sprintf(answers + pos,  "{b}"); pos += bytes;
		}
		else if (randomizer% 4 == 2)
		{
			bytes = sprintf(answers + pos,  "{a}"); pos += bytes;
		}
		else
		{
			bytes = sprintf(answers + pos,  "{b}"); pos += bytes;
		}

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_2_30(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		A = rand() % 19 - 9;

		do
		{
			B = rand() % 19 - 9;
		} while (B == A);

		do
		{
			C = rand() % 19 - 9;
		} while (C == A || C == B);

		do
		{
			D = rand() % 19 - 9;
		} while (D == A || D == B || D == C);

		do
		{		
			E = rand() % 19 - 9;
		} while (E == A || E == B || E == C || E == D);

		do
		{
			F = rand() % 19 - 9;
		} while (F == A || F == B || F == C || F == D || F == E);

		do
		{
			G = rand() % 19 - 9;
		} while (G == A || G == B || G == C || G == D || G == E || G == F);

		do	
		{
			H = rand() % 19 - 9;
		} while (H == A || H == B || H == C || H == D || H == E || H == F || H == G);


		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Use the table to evaluate the given composite function.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p><table border='1'><tr><td>x</td><td>f(x)</td><td>g(x)</td></tr>"); pos += bytes;
		bytes = sprintf(content + pos, "<tr><td>%d</td><td>%d</td><td>%d</td></tr>", A, D, B); pos += bytes;
		bytes = sprintf(content + pos, "<tr><td>%d</td><td>%d</td><td>%d</td></tr>", B, C, E); pos += bytes;
		bytes = sprintf(content + pos, "<tr><td>%d</td><td>%d</td><td>%d</td></tr>", D, F, G); pos += bytes;
		bytes = sprintf(content + pos, "<tr><td>%d</td><td>%d</td><td>%d</td></tr>", G, E, H); pos += bytes;
		bytes = sprintf(content + pos, "<tr><td>%d</td><td>%d</td><td>%d</td></tr>", C, G, D); pos += bytes;
		bytes = sprintf(content + pos, "<tr><td>%d</td><td>%d</td><td>%d</td></tr>", H, A, F); pos += bytes;
		bytes = sprintf(content + pos, "<tr><td>%d</td><td>%d</td><td>%d</td></tr>", F, B, A); pos += bytes;
		bytes = sprintf(content + pos, "<tr><td>%d</td><td>%d</td><td>%d</td></tr>", E, H, C); pos += bytes;
		bytes = sprintf(content + pos, "</table></p>"); pos += bytes;

		int start = 0, end = 0;

		if (randomizer% 8 == 0)
		{		
			start = A; end = C;
		}
		else if (randomizer% 8 == 1)
		{
			start = B; end = H;
		}
		else if (randomizer% 8 == 2)
		{
			start = D; end = E;
		}
		else if (randomizer% 8 == 3)
		{
			start = G; end = A;
		}
		else if (randomizer% 8 == 4)
		{
			start = C; end = F;
		}
		else if (randomizer% 8 == 5)
		{
			start = H; end = B;
		}
		else if (randomizer% 8 == 6)
		{
			start = F; end = D;
		}
		else
		{
			start = E; end = G;
		}
		
		bytes = sprintf(content + pos, "<p>\\((f\\circ g)(%d) =\\ \\ ?\\)</p>", start); pos += bytes;
	
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%d", end); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;	

	return 1;
};

int question_3_01(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 18 + 2;
			B = rand() % 8 + 2;
			C = rand() % 8 + 2;
			D = rand() % 18 + 2;
			E = rand() % 18 + 2;
			F = rand() % 9 - 4;
		} while (F == 0 || F == 1 || B == C || B == F || C == F ||
			!isprime(D) || !isprime(E) || D == E);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Determine whether the function is a polynomial function.</p>"); pos += bytes;
		if (randomizer % 3 == 0)
		{
			bytes = sprintf(content + pos, "<p>\\(g(x) = %dx^{%d}+%dx^{%d}-%dx^{%d}\\)</p>",
				A, B, D, C, E, F); pos += bytes;
		}
		else if (randomizer % 3 == 1)
		{
			bytes = sprintf(content + pos, "<p>\\(g(x) = %dx^{%d}+x^{%d}-\\dfrac{ %d }{ %d }x^{%d}\\)</p>",
				A, B, C, D, E, F); pos += bytes;
		}
		else
		{
			bytes = sprintf(content + pos, "<p>\\(g(x) = %dx^{%d}+%d\\pi x^{%d}-%dx^{%d}\\)</p>",
				A, B, D, C, E, F); pos += bytes;
		}
		bytes = sprintf(content + pos, "<p>Is the function polynomial?</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>a) Yes</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>b) No</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		if (F > 0)
		{
			bytes = sprintf(answers + pos,  "{a}"); pos += bytes;
		}
		else
		{
			bytes = sprintf(answers + pos,  "{b}"); pos += bytes;
		}

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_3_02(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 19 - 9;
			B = rand() % 8 + 2;
			C = rand() % 19 - 9;
			D = rand() % 8 + 2;
			E = rand() % 19 - 9;
			F = rand() % 8 + 2;
		} while (fabs(A) <= 1.0f || fabs(C) <= 1.0f || fabs(E) <= 1.0f || B <= D || B <= F || D <= F);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Use the leading coefficient test to determine the end behavior of the graph of the polynomial function.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(g(x) = %dx^{%d}%c%dx^{%d}%c%dx^{%d}\\)</p>",
			A, B, (C>0?'+':'-'), (int)fabs(C), D, (E>0?'+':'-'), (int)fabs(E), F); pos += bytes;
		bytes = sprintf(content + pos, "<p>a) Rises left & rises right.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>b) Falls left & falls right.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>c) Falls left & rises right.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>d) Rises left & falls right.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		if (A > 0 && B % 2 == 0)
		{
			bytes = sprintf(answers + pos,  "{a}"); pos += bytes;
		}
		else if (A < 0 && B % 2 == 0)
		{
			bytes = sprintf(answers + pos,  "{b}"); pos += bytes;
		}
		else if (A > 0 && B % 2 == 1)
		{
			bytes = sprintf(answers + pos,  "{c}"); pos += bytes;
		}
		else if (A < 0 && B % 2 == 1)
		{
			bytes = sprintf(answers + pos,  "{d}"); pos += bytes;
		}

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_3_03(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 19 - 9;
			B = rand() % 19 - 9;
		} while (A == 0 || B == 0 || A == B || fabs(-A-B) <= 1.0f || fabs(A*B) <= 1.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Find the zeros of the given polynomial function.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(f(x) = x^{3}%c%dx^{2}%c%dx\\)</p>",
			(-A-B>0?'+':'-'), (int)fabs(-A-B), (A*B>0?'+':'-'), (int)fabs(A*B)); pos += bytes;
		bytes = sprintf(content + pos, "<p>Separate answers with a comma.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "0,%d,%d\t", A, B); pos += bytes;
		bytes = sprintf(answers + pos,  "0,%d,%d\t", B, A); pos += bytes;
		bytes = sprintf(answers + pos,  "%d,0,%d\t", A, B); pos += bytes;
		bytes = sprintf(answers + pos,  "%d,0,%d\t", B, A); pos += bytes;
		bytes = sprintf(answers + pos,  "%d,%d,0\t", A, B); pos += bytes;
		bytes = sprintf(answers + pos,  "%d,%d,0", B, A); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_3_04(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 19 - 9;
		} while (fabs(A) <= 1.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Find the zeros of the given polynomial function.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(f(x) = x^{3}%c%dx^{2}%c%dx\\)</p>",
			(-A-A>0?'+':'-'), (int)fabs(-A-A), (A*A>0?'+':'-'), (int)fabs(A*A)); pos += bytes;
		bytes = sprintf(content + pos, "<p>Separate answers with a comma.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "0,%d,%d\t", A, A); pos += bytes;
		bytes = sprintf(answers + pos,  "%d,0,%d\t", A, A); pos += bytes;
		bytes = sprintf(answers + pos,  "%d,%d,0\t", A, A); pos += bytes;
		bytes = sprintf(answers + pos,  "0,%d\t", A); pos += bytes;
		bytes = sprintf(answers + pos,  "%d,0", A); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_3_05(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 19 - 9;
		} while (fabs(A) <= 1.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		if (randomizer% 2 == 0)
		{
			bytes = sprintf(content + pos, "<p>Find the multiplicity of the larger zero of the given polynomial function.</p>"); pos += bytes;
		}
		else
		{
			bytes = sprintf(content + pos, "<p>Find the multiplicity of the smaller zero of the given polynomial function.</p>"); pos += bytes;
		}
		bytes = sprintf(content + pos, "<p>\\(f(x) = x^{3}%c%dx^{2}%c%dx\\)</p>",
			(-A-A>0?'+':'-'), (int)fabs(-A-A), (A*A>0?'+':'-'), (int)fabs(A*A)); pos += bytes;

		bytes = 0;
		pos = 0;			

		if (randomizer% 2 == 0)
		{
			if (A > 0) { bytes = sprintf(answers + pos,  "2"); pos += bytes; }
			else { bytes = sprintf(answers + pos,  "1"); pos += bytes; }
		}
		else
		{
			if (A < 0) { bytes = sprintf(answers + pos,  "2"); pos += bytes; }
			else { bytes = sprintf(answers + pos,  "1"); pos += bytes; }
		}

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_3_06(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 19 - 9;
		} while (fabs(A) <= 1.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		if (randomizer% 2 == 0)
		{
			bytes = sprintf(content + pos, "<p>Determine if the larger zero of the given polynomial function touches the x-axis then turns around or crosses through the x-axis.</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>\\(f(x) = x^{3}%c%dx^{2}%c%dx\\)</p>",
				(-A-A>0?'+':'-'), (int)fabs(-A-A), (A*A>0?'+':'-'), (int)fabs(A*A)); pos += bytes;
			bytes = sprintf(content + pos, "<p>The larger zero:</p>"); pos += bytes;		
		}
		else
		{
			bytes = sprintf(content + pos, "<p>Determine if the smaller zero of the given polynomial function touches the x-axis then turns around or crosses through the x-axis.</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>\\(f(x) = x^{3}%c%dx^{2}%c%dx\\)</p>",
				(-A-A>0?'+':'-'), (int)fabs(-A-A), (A*A>0?'+':'-'), (int)fabs(A*A)); pos += bytes;
			bytes = sprintf(content + pos, "<p>The smaller zero:</p>"); pos += bytes;		
		}
		bytes = sprintf(content + pos, "<p>a) Touches the x-axis then turns around</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>b) Crosses through the x-axis</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		if (randomizer% 2 == 0)
		{
			if (A > 0) { bytes = sprintf(answers + pos,  "{a}"); pos += bytes; }
			else { bytes = sprintf(answers + pos,  "{b}"); pos += bytes; }
		}
		else
		{
			if (A < 0) { bytes = sprintf(answers + pos,  "{a}"); pos += bytes; }
			else { bytes = sprintf(answers + pos,  "{b}"); pos += bytes; }
		}

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_3_07(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 8 + 2;
			B = rand() % 19 - 9;
			C = rand() % 19 - 9;
			D = rand() % 8 + 2;
			E = rand() % 19 - 9;
		} while (B == 0 || C == 0 || E == 0 || fabs(A*E+B*D) <= 1.0f || fabs(B*E+C*D) <= 1.0f || C*E == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Divide using long division.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\( \\dfrac{ %dx^{3}%c%dx^{2}%c%dx%c%d }{ %dx%c%d } =\\ \\ ? \\)</p>",
			A*D, (A*E+B*D>0?'+':'-'), (int)fabs(A*E+B*D), (B*E+C*D>0?'+':'-'), (int)fabs(B*E+C*D),
			(C*E>0?'+':'-'), (int)fabs(C*E), D, (E>0?'+':'-'), (int)fabs(E)); pos += bytes;	

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%dx{^{`2`}}%c%dx%c%d\t", 
			A, (B>0?'+':'-'), (int)fabs(B), (C>0?'+':'-'), (int)fabs(C)); pos += bytes;
		bytes = sprintf(answers + pos,  "%dx+%dx{^{`2`}}%c%d\t", 
			B, A, (C>0?'+':'-'), (int)fabs(C)); pos += bytes;
		bytes = sprintf(answers + pos,  "%dx%c%d+%dx{^{`2`}}\t", 
			B, (C>0?'+':'-'), (int)fabs(C), A); pos += bytes;
		bytes = sprintf(answers + pos,  "%dx{^{`2`}}%c%d%c%dx\t", 
			A, (C>0?'+':'-'), (int)fabs(C), (B>0?'+':'-'), (int)fabs(B)); pos += bytes;
		bytes = sprintf(answers + pos,  "%d+%dx{^{`2`}}%c%dx\t", 
			C, A, (B>0?'+':'-'), (int)fabs(B)); pos += bytes;
		bytes = sprintf(answers + pos,  "%d%c%dx+%dx{^{`2`}}", 
			C, (B>0?'+':'-'), (int)fabs(B), A); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_3_08(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 19 - 9;
			B = rand() % 19 - 9;
			C = rand() % 19 - 9;
			D = rand() % 19 - 9;
		} while (B == 0 || C == 0 || D == 0 || fabs(-D*D+A) <= 1.0f || fabs(A*D+B) <= 1.0f || fabs(B*D+C) <= 1.0f ||
			fabs(D) <= 1.0f || fabs(A) <= 1.0f || fabs(B) <= 1.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Divide using synthetic division.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\( \\dfrac{ x^{5}%c%dx^{3}%c%dx^{2}%c%dx%c%d }{ x%c%d } =\\ \\ ? \\)</p>",
			(-D*D+A>0?'+':'-'), (int)fabs(-D*D+A), (A*D+B>0?'+':'-'), (int)fabs(A*D+B),
			(B*D+C>0?'+':'-'), (int)fabs(B*D+C), (C*D>0?'+':'-'), (int)fabs(C*D), (D>0?'+':'-'), (int)fabs(D)); pos += bytes;	

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "x{^{`4`}}%c%dx{^{`3`}}%c%dx{^{`2`}}%c%dx%c%d", 
			(-D>0?'+':'-'), (int)fabs(-D), (A>0?'+':'-'), (int)fabs(A), (B>0?'+':'-'), (int)fabs(B),
			(C>0?'+':'-'), (int)fabs(C)); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;
	
	return 1;
};

int question_3_09(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 8 + 2;
			B = rand() % 19 - 9;
			C = rand() % 19 - 9;
			D = rand() % 19 - 9;
			E = rand() % 19 - 9;
			F = rand() % 19 - 9;
		} while (B == 0 || C == 0 || D == 0 || E == 0 || F == 0 || fabs(B) <= 1.0f || fabs(C) <= 1.0f || fabs(D) <= 1.0f || fabs(E) <= 1.0f ||
			A*F*F*F*F+B*F*F*F+C*F*F+D*F+E == 0 || fabs(A*F*F*F*F+B*F*F*F+C*F*F+D*F+E) >= 1000.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Use synthetic division and Remainder Theorem to find the indicated function value.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(f(x)=%dx^{4}%c%dx^{3}%c%dx^{2}%c%dx%c%d\\)</p><p>\\(f(%d) =\\ \\ ?\\)</p>",
			A, (B>0?'+':'-'), (int)fabs(B), (C>0?'+':'-'), (int)fabs(C), (D>0?'+':'-'), (int)fabs(D), 
			(E>0?'+':'-'), (int)fabs(E), F); pos += bytes;	

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%d", A*F*F*F*F+B*F*F*F+C*F*F+D*F+E); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;	

	return 1;
};

int question_3_10(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 19 - 9;
			B = rand() % 19 - 9;
			C = rand() % 19 - 9;
		} while (A == 0 || B == 0 || C == 0 || A == B || A == C || B == C || fabs(A+B+C) <= 1.0f || fabs(A*C+B*C+A*B) <= 1.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve the equation given that \\(%d\\) is a zero.</p>", -A); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(x^{3}%c%dx^{2}%c%dx%c%d=0\\)</p><p>\\(x\\ =\\ ?\\)</p>",
			(A+B+C>0?'+':'-'), (int)fabs(A+B+C), (A*C+B*C+A*B>0?'+':'-'), (int)fabs(A*C+B*C+A*B), 
			(A*B*C>0?'+':'-'), (int)fabs(A*B*C)); pos += bytes;
		bytes = sprintf(content + pos, "<p>Separate answers with commas.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%d,%d,%d\t", -A, -B, -C); pos += bytes;
		bytes = sprintf(answers + pos,  "%d,%d,%d\t", -A, -C, -B); pos += bytes;
		bytes = sprintf(answers + pos,  "%d,%d,%d\t", -B, -A, -C); pos += bytes;
		bytes = sprintf(answers + pos,  "%d,%d,%d\t", -B, -C, -A); pos += bytes;
		bytes = sprintf(answers + pos,  "%d,%d,%d\t", -C, -A, -B); pos += bytes;
		bytes = sprintf(answers + pos,  "%d,%d,%d", -C, -B, -A); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;		

	return 1;
};

int question_3_11(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = (rand() % 19 - 9);
			B = (rand() % 19 - 9);
			C = (rand() % 19 - 9);
			D = (rand() % 19 - 9);
		} while (fabs(A) <= 1.0f || fabs(B) <= 1.0f || fabs(C) <= 1.0f || fabs(-A*D+B) <= 1.0f || 
			fabs(-B*D+C) <= 1.0f || fabs(-C*D) <= 1.0f || D == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Given the area and width of a rectangle, find the length.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>Area = \\(%dx^{3}%c%dx^{2}%c%dx%c%d\\)</p><p>Width = \\(x%c%d\\)</p>",
			A, ((-A*D+B)>0.0f?'+':'-'), (int)fabs((-A*D+B)), 
			((-B*D+C)>0.0f?'+':'-'), (int)fabs((-B*D+C)), ((-C*D)>0.0f?'+':'-'), (int)fabs((-C*D)),
			(-D>0.0f?'+':'-'), (int)fabs(-D)); pos += bytes;
		bytes = sprintf(content + pos, "<p>Length =\\(?\\)</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%dx{^{`2`}}%c%dx%c%d", A, (B>0.0f?'+':'-'), (int)fabs(B),
			(C>0.0f?'+':'-'), (int)fabs(C)); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;		

	return 1;
};

int question_3_12(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 18 + 3;
			B = rand() % 39 - 19;
			C = rand() % 39 - 19;
			D = rand() % 39 - 19;
			F = rand() % 3;
			if (F == 0) F = 8;
			else if (F == 1) F = 10;
			else if (F == 2) F = 14;
		} while (fabs(A) <= 1.0f || fabs(B) <= 1.0f || fabs(C) <= 1.0f || fabs(D) <= 1.0f || fabs(E) <= 1.0f || F == 0 || 
			!isprime(A) || A == 2 || A == 5 || A == 7);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Use the Rational Zero Theorem to list all possible rational zeros for \\(f(x)\\).</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\( f(x) = %dx^{4}%c%dx^{3}%c%dx^{2}%c%dx%c%d \\)</p>",
			A, (B>0?'+':'-'), (int)fabs(B), (C>0?'+':'-'), (int)fabs(C), (D>0?'+':'-'), (int)fabs(D),
			(F>0?'+':'-'), (int)fabs(F)); pos += bytes;
		bytes = sprintf(content + pos, "<p>Choose the answer below:</p>"); pos += bytes;
		
		if (randomizer% 4 == 0)
		{
			if (F == 8)
			{
				bytes = sprintf(content + pos, "<p>a) \\({\\pm}1,{\\pm}2,{\\pm}4,{\\pm}8,"); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{1}{%d},{\\pm}\\dfrac{2}{%d},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{4}{%d},{\\pm}\\dfrac{8}{%d}\\)</p>", A, A); pos += bytes;
				
				bytes = sprintf(content + pos, "<p>b) \\({\\pm}1,{\\pm}2,{\\pm}4,{\\pm}8,"); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{1},{\\pm}\\dfrac{%d}{2},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{4},{\\pm}\\dfrac{%d}{8}\\)</p>", A, A); pos += bytes;

				bytes = sprintf(content + pos, "<p>c) \\({\\pm}1,{\\pm}%d,", A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{1},{\\pm}\\dfrac{%d}{2},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{4},{\\pm}\\dfrac{%d}{8}\\)</p>", A, A); pos += bytes;

				bytes = sprintf(content + pos, "<p>d) \\({\\pm}1,{\\pm}%d,", A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{1}{%d},{\\pm}\\dfrac{2}{%d},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{4}{%d},{\\pm}\\dfrac{8}{%d}\\)</p>", A, A); pos += bytes;
			}
			else if (F == 10)
			{
				bytes = sprintf(content + pos, "<p>a) \\({\\pm}1,{\\pm}2,{\\pm}5,{\\pm}10,"); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{1}{%d},{\\pm}\\dfrac{2}{%d},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{5}{%d},{\\pm}\\dfrac{10}{%d}\\)</p>", A, A); pos += bytes;
				
				bytes = sprintf(content + pos, "<p>b) \\({\\pm}1,{\\pm}2,{\\pm}5,{\\pm}10,"); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{1},{\\pm}\\dfrac{%d}{2},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{5},{\\pm}\\dfrac{%d}{10}\\)</p>", A, A); pos += bytes;

				bytes = sprintf(content + pos, "<p>c) \\({\\pm}1,{\\pm}%d,", A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{1},{\\pm}\\dfrac{%d}{2},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{5},{\\pm}\\dfrac{%d}{10}\\)</p>", A, A); pos += bytes;

				bytes = sprintf(content + pos, "<p>d) \\({\\pm}1,{\\pm}%d,", A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{1}{%d},{\\pm}\\dfrac{2}{%d},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{5}{%d},{\\pm}\\dfrac{10}{%d}\\)</p>", A, A); pos += bytes;
			}
			else if (F == 14)
			{
				bytes = sprintf(content + pos, "<p>a) \\({\\pm}1,{\\pm}2,{\\pm}7,{\\pm}14,"); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{1}{%d},{\\pm}\\dfrac{2}{%d},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{7}{%d},{\\pm}\\dfrac{14}{%d}\\)</p>", A, A); pos += bytes;
				
				bytes = sprintf(content + pos, "<p>b) \\({\\pm}1,{\\pm}2,{\\pm}7,{\\pm}14,"); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{1},{\\pm}\\dfrac{%d}{2},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{7},{\\pm}\\dfrac{%d}{14}\\)</p>", A, A); pos += bytes;

				bytes = sprintf(content + pos, "<p>c) \\({\\pm}1,{\\pm}%d,", A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{1},{\\pm}\\dfrac{%d}{2},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{7},{\\pm}\\dfrac{%d}{14}\\)</p>", A, A); pos += bytes;

				bytes = sprintf(content + pos, "<p>d) \\({\\pm}1,{\\pm}%d,", A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{1}{%d},{\\pm}\\dfrac{2}{%d},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{7}{%d},{\\pm}\\dfrac{14}{%d}\\)</p>", A, A); pos += bytes;
			}
		}
		else if (randomizer% 4 == 1)
		{
			if (F == 8)
			{	
				bytes = sprintf(content + pos, "<p>a) \\({\\pm}1,{\\pm}2,{\\pm}4,{\\pm}8,"); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{1},{\\pm}\\dfrac{%d}{2},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{4},{\\pm}\\dfrac{%d}{8}\\)</p>", A, A); pos += bytes;

				bytes = sprintf(content + pos, "<p>b) \\({\\pm}1,{\\pm}2,{\\pm}4,{\\pm}8,"); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{1}{%d},{\\pm}\\dfrac{2}{%d},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{4}{%d},{\\pm}\\dfrac{8}{%d}\\)</p>", A, A); pos += bytes;

				bytes = sprintf(content + pos, "<p>c) \\({\\pm}1,{\\pm}%d,", A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{1},{\\pm}\\dfrac{%d}{2},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{4},{\\pm}\\dfrac{%d}{8}\\)</p>", A, A); pos += bytes;

				bytes = sprintf(content + pos, "<p>d) \\({\\pm}1,{\\pm}%d,", A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{1}{%d},{\\pm}\\dfrac{2}{%d},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{4}{%d},{\\pm}\\dfrac{8}{%d}\\)</p>", A, A); pos += bytes;
			}
			else if (F == 10)
			{	
				bytes = sprintf(content + pos, "<p>a) \\({\\pm}1,{\\pm}2,{\\pm}5,{\\pm}10,"); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{1},{\\pm}\\dfrac{%d}{2},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{5},{\\pm}\\dfrac{%d}{10}\\)</p>", A, A); pos += bytes;

				bytes = sprintf(content + pos, "<p>b) \\({\\pm}1,{\\pm}2,{\\pm}5,{\\pm}10,"); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{1}{%d},{\\pm}\\dfrac{2}{%d},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{5}{%d},{\\pm}\\dfrac{10}{%d}\\)</p>", A, A); pos += bytes;

				bytes = sprintf(content + pos, "<p>c) \\({\\pm}1,{\\pm}%d,", A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{1},{\\pm}\\dfrac{%d}{2},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{5},{\\pm}\\dfrac{%d}{10}\\)</p>", A, A); pos += bytes;

				bytes = sprintf(content + pos, "<p>d) \\({\\pm}1,{\\pm}%d,", A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{1}{%d},{\\pm}\\dfrac{2}{%d},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{5}{%d},{\\pm}\\dfrac{10}{%d}\\)</p>", A, A); pos += bytes;
			}
			else if (F == 14)
			{
				bytes = sprintf(content + pos, "<p>a) \\({\\pm}1,{\\pm}2,{\\pm}7,{\\pm}14,"); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{1},{\\pm}\\dfrac{%d}{2},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{7},{\\pm}\\dfrac{%d}{14}\\)</p>", A, A); pos += bytes;

				bytes = sprintf(content + pos, "<p>b) \\({\\pm}1,{\\pm}2,{\\pm}7,{\\pm}14,"); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{1}{%d},{\\pm}\\dfrac{2}{%d},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{7}{%d},{\\pm}\\dfrac{14}{%d}\\)</p>", A, A); pos += bytes;

				bytes = sprintf(content + pos, "<p>c) \\({\\pm}1,{\\pm}%d,", A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{1},{\\pm}\\dfrac{%d}{2},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{7},{\\pm}\\dfrac{%d}{14}\\)</p>", A, A); pos += bytes;

				bytes = sprintf(content + pos, "<p>d) \\({\\pm}1,{\\pm}%d,", A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{1}{%d},{\\pm}\\dfrac{2}{%d},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{7}{%d},{\\pm}\\dfrac{14}{%d}\\)</p>", A, A); pos += bytes;
			}
		}
		else if (randomizer% 4 == 2)
		{
			if (F == 8)
			{	
				bytes = sprintf(content + pos, "<p>a) \\({\\pm}1,{\\pm}2,{\\pm}4,{\\pm}8,"); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{1},{\\pm}\\dfrac{%d}{2},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{4},{\\pm}\\dfrac{%d}{8}\\)</p>", A, A); pos += bytes;

				bytes = sprintf(content + pos, "<p>b) \\({\\pm}1,{\\pm}%d,", A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{1}{%d},{\\pm}\\dfrac{2}{%d},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{4}{%d},{\\pm}\\dfrac{8}{%d}\\)</p>", A, A); pos += bytes;

				bytes = sprintf(content + pos, "<p>c) \\({\\pm}1,{\\pm}2,{\\pm}4,{\\pm}8,"); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{1}{%d},{\\pm}\\dfrac{2}{%d},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{4}{%d},{\\pm}\\dfrac{8}{%d}\\)</p>", A, A); pos += bytes;

				bytes = sprintf(content + pos, "<p>d) \\({\\pm}1,{\\pm}%d,", A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{1},{\\pm}\\dfrac{%d}{2},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{4},{\\pm}\\dfrac{%d}{8}\\)</p>", A, A); pos += bytes;
			}
			else if (F == 10)
			{	
				bytes = sprintf(content + pos, "<p>a) \\({\\pm}1,{\\pm}2,{\\pm}5,{\\pm}10,"); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{1},{\\pm}\\dfrac{%d}{2},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{5},{\\pm}\\dfrac{%d}{10}\\)</p>", A, A); pos += bytes;

				bytes = sprintf(content + pos, "<p>b) \\({\\pm}1,{\\pm}%d,", A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{1}{%d},{\\pm}\\dfrac{2}{%d},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{5}{%d},{\\pm}\\dfrac{10}{%d}\\)</p>", A, A); pos += bytes;

				bytes = sprintf(content + pos, "<p>c) \\({\\pm}1,{\\pm}2,{\\pm}5,{\\pm}10,"); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{1}{%d},{\\pm}\\dfrac{2}{%d},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{5}{%d},{\\pm}\\dfrac{10}{%d}\\)</p>", A, A); pos += bytes;

				bytes = sprintf(content + pos, "<p>d) \\({\\pm}1,{\\pm}%d,", A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{1},{\\pm}\\dfrac{%d}{2},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{5},{\\pm}\\dfrac{%d}{10}\\)</p>", A, A); pos += bytes;
			}
			else if (F == 14)
			{
				bytes = sprintf(content + pos, "<p>a) \\({\\pm}1,{\\pm}2,{\\pm}7,{\\pm}14,"); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{1},{\\pm}\\dfrac{%d}{2},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{7},{\\pm}\\dfrac{%d}{14}\\)</p>", A, A); pos += bytes;

				bytes = sprintf(content + pos, "<p>b) \\({\\pm}1,{\\pm}%d,", A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{1}{%d},{\\pm}\\dfrac{2}{%d},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{7}{%d},{\\pm}\\dfrac{14}{%d}\\)</p>", A, A); pos += bytes;

				bytes = sprintf(content + pos, "<p>c) \\({\\pm}1,{\\pm}2,{\\pm}7,{\\pm}14,"); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{1}{%d},{\\pm}\\dfrac{2}{%d},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{7}{%d},{\\pm}\\dfrac{14}{%d}\\)</p>", A, A); pos += bytes;

				bytes = sprintf(content + pos, "<p>d) \\({\\pm}1,{\\pm}%d,", A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{1},{\\pm}\\dfrac{%d}{2},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{7},{\\pm}\\dfrac{%d}{14}\\)</p>", A, A); pos += bytes;
			}
		}
		else if (randomizer% 4 == 3)
		{
			if (F == 8)
			{	
				bytes = sprintf(content + pos, "<p>a) \\({\\pm}1,{\\pm}%d,", A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{1}{%d},{\\pm}\\dfrac{2}{%d},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{4}{%d},{\\pm}\\dfrac{8}{%d}\\)</p>", A, A); pos += bytes;

				bytes = sprintf(content + pos, "<p>b) \\({\\pm}1,{\\pm}2,{\\pm}4,{\\pm}8,"); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{1},{\\pm}\\dfrac{%d}{2},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{4},{\\pm}\\dfrac{%d}{8}\\)</p>", A, A); pos += bytes;

				bytes = sprintf(content + pos, "<p>c) \\({\\pm}1,{\\pm}%d,", A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{1},{\\pm}\\dfrac{%d}{2},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{4},{\\pm}\\dfrac{%d}{8}\\)</p>", A, A); pos += bytes;

				bytes = sprintf(content + pos, "<p>d) \\({\\pm}1,{\\pm}2,{\\pm}4,{\\pm}8,"); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{1}{%d},{\\pm}\\dfrac{2}{%d},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{4}{%d},{\\pm}\\dfrac{8}{%d}\\)</p>", A, A); pos += bytes;
			}
			else if (F == 10)
			{	
				bytes = sprintf(content + pos, "<p>a) \\({\\pm}1,{\\pm}%d,", A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{1}{%d},{\\pm}\\dfrac{2}{%d},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{5}{%d},{\\pm}\\dfrac{10}{%d}\\)</p>", A, A); pos += bytes;

				bytes = sprintf(content + pos, "<p>b) \\({\\pm}1,{\\pm}2,{\\pm}5,{\\pm}10,"); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{1},{\\pm}\\dfrac{%d}{2},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{5},{\\pm}\\dfrac{%d}{10}\\)</p>", A, A); pos += bytes;

				bytes = sprintf(content + pos, "<p>c) \\({\\pm}1,{\\pm}%d,", A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{1},{\\pm}\\dfrac{%d}{2},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{5},{\\pm}\\dfrac{%d}{10}\\)</p>", A, A); pos += bytes;
	
				bytes = sprintf(content + pos, "<p>d) \\({\\pm}1,{\\pm}2,{\\pm}5,{\\pm}10,"); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{1}{%d},{\\pm}\\dfrac{2}{%d},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{5}{%d},{\\pm}\\dfrac{10}{%d}\\)</p>", A, A); pos += bytes;
			}
			else if (F == 14)
			{
				bytes = sprintf(content + pos, "<p>a) \\({\\pm}1,{\\pm}%d,", A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{1}{%d},{\\pm}\\dfrac{2}{%d},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{7}{%d},{\\pm}\\dfrac{14}{%d}\\)</p>", A, A); pos += bytes;

				bytes = sprintf(content + pos, "<p>b) \\({\\pm}1,{\\pm}2,{\\pm}7,{\\pm}14,"); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{1},{\\pm}\\dfrac{%d}{2},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{7},{\\pm}\\dfrac{%d}{14}\\)</p>", A, A); pos += bytes;

				bytes = sprintf(content + pos, "<p>c) \\({\\pm}1,{\\pm}%d,", A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{1},{\\pm}\\dfrac{%d}{2},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{%d}{7},{\\pm}\\dfrac{%d}{14}\\)</p>", A, A); pos += bytes;

				bytes = sprintf(content + pos, "<p>d) \\({\\pm}1,{\\pm}2,{\\pm}7,{\\pm}14,"); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{1}{%d},{\\pm}\\dfrac{2}{%d},", A, A); pos += bytes;
				bytes = sprintf(content + pos, "{\\pm}\\dfrac{7}{%d},{\\pm}\\dfrac{14}{%d}\\)</p>", A, A); pos += bytes;
			}
		}

		bytes = 0;
		pos = 0;			

		if (randomizer% 4 == 0)
		{
			bytes = sprintf(answers + pos,  "{a}"); pos += bytes;
		}
		else if (randomizer% 4 == 1)
		{
			bytes = sprintf(answers + pos,  "{b}"); pos += bytes;
		}
		else if (randomizer% 4 == 2)
		{
			bytes = sprintf(answers + pos,  "{c}"); pos += bytes;
		}
		else if (randomizer% 4 == 3)
		{
			bytes = sprintf(answers + pos,  "{d}"); pos += bytes;
		}

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;		


	return 1;
};

int question_3_13(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 39 - 19;
			B = rand() % 39 - 19;
			C = rand() % 39 - 19;
			D = rand() % 39 - 19;
			E = rand() % 5;
			F = rand() % 2;
			if (E == 0) E = 15; // 3 * 5
			else if (E == 1) E = 21; // 3 * 7
			else if (E == 2) E = 33; // 3 * 11
			else if (E == 3) E = 39; // 3 * 13
			else if (E == 4) E = 51; // 3 * 17
			if (F == 0) E = -E;
		} while (fabs(A) <= 1.0f || fabs(B) <= 1.0f || fabs(C) <= 1.0f || fabs(D) <= 1.0f || E == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Use the Rational Zero Theorem to list all possible rational zeros for \\(f(x)\\).</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\( f(x) = x^{5} %c%d x^{4} %c%d x^{3} %c%d x^{2} %c%d x %c%d \\)</p>",
			(A>0?'+':'-'), (int)fabs(A), (B>0?'+':'-'), (int)fabs(B), (C>0?'+':'-'), (int)fabs(C), (D>0?'+':'-'), (int)fabs(D),
			(E>0?'+':'-'), (int)fabs(E)); pos += bytes;
		bytes = sprintf(content + pos, "<p>Choose the answer below:</p>"); pos += bytes;
		
		if (randomizer% 4 == 0)
		{
			bytes = sprintf(content + pos, "<p>a) \\({\\pm}1,{\\pm}3,{\\pm}%d,{\\pm}%d\\)</p>", 
				(int)fabs(E/3), (int)fabs(E)); pos += bytes;
				
			bytes = sprintf(content + pos, "<p>b) \\({\\pm}1,{\\pm}2,{\\pm}%d,{\\pm}%d\\)</p>", 
				(int)fabs((E-1)/2), (int)fabs(E)); pos += bytes;

			bytes = sprintf(content + pos, "<p>c) \\({\\pm}1,{\\pm}3,{\\pm}%d\\)</p>", 
				(int)fabs(E)); pos += bytes;
	
			bytes = sprintf(content + pos, "<p>d) \\({\\pm}1,{\\pm}2,{\\pm}%d\\)</p>", 
				(int)fabs(E)); pos += bytes;
		}
		else if (randomizer% 4 == 1)
		{
				
			bytes = sprintf(content + pos, "<p>a) \\({\\pm}1,{\\pm}2,{\\pm}%d,{\\pm}%d\\)</p>", 
				(int)fabs((E-1)/2), (int)fabs(E)); pos += bytes;

			bytes = sprintf(content + pos, "<p>b) \\({\\pm}1,{\\pm}3,{\\pm}%d,{\\pm}%d\\)</p>", 
				(int)fabs(E/3), (int)fabs(E)); pos += bytes;

			bytes = sprintf(content + pos, "<p>c) \\({\\pm}1,{\\pm}2,{\\pm}%d\\)</p>", 
				(int)fabs(E)); pos += bytes;

			bytes = sprintf(content + pos, "<p>d) \\({\\pm}1,{\\pm}3,{\\pm}%d\\)</p>", 
				(int)fabs(E)); pos += bytes;
		}
		else if (randomizer% 4 == 2)
		{
				
			bytes = sprintf(content + pos, "<p>a) \\({\\pm}1,{\\pm}2,{\\pm}%d\\)</p>", 
				(int)fabs(E)); pos += bytes;

			bytes = sprintf(content + pos, "<p>b) \\({\\pm}1,{\\pm}3,{\\pm}%d\\)</p>", 
				(int)fabs(E)); pos += bytes;

			bytes = sprintf(content + pos, "<p>c) \\({\\pm}1,{\\pm}3,{\\pm}%d,{\\pm}%d\\)</p>", 
				(int)fabs(E/3), (int)fabs(E)); pos += bytes;
		
			bytes = sprintf(content + pos, "<p>d) \\({\\pm}1,{\\pm}2,{\\pm}%d,{\\pm}%d\\)</p>", 
				(int)fabs((E-1)/2), (int)fabs(E)); pos += bytes;
		}
		else if (randomizer% 4 == 3)
		{
			bytes = sprintf(content + pos, "<p>a) \\({\\pm}1,{\\pm}3,{\\pm}%d\\)</p>", 
				(int)fabs(E)); pos += bytes;		
		
			bytes = sprintf(content + pos, "<p>b) \\({\\pm}1,{\\pm}2,{\\pm}%d\\)</p>", 
				(int)fabs(E)); pos += bytes;

			bytes = sprintf(content + pos, "<p>c) \\({\\pm}1,{\\pm}2,{\\pm}%d,{\\pm}%d\\)</p>", 
				(int)fabs((E-1)/2), (int)fabs(E)); pos += bytes;

			bytes = sprintf(content + pos, "<p>d) \\({\\pm}1,{\\pm}3,{\\pm}%d,{\\pm}%d\\)</p>", 
				(int)fabs(E/3), (int)fabs(E)); pos += bytes;
		}
	
		bytes = 0;
		pos = 0;			

		if (randomizer% 4 == 0)
		{
			bytes = sprintf(answers + pos,  "{a}"); pos += bytes;
		}
		else if (randomizer% 4 == 1)
		{
			bytes = sprintf(answers + pos,  "{b}"); pos += bytes;
		}
		else if (randomizer% 4 == 2)
		{
			bytes = sprintf(answers + pos,  "{c}"); pos += bytes;
		}
		else if (randomizer% 4 == 3)
		{
			bytes = sprintf(answers + pos,  "{d}"); pos += bytes;
		}

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;		

	return 1;
};

int question_3_14(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 19 - 9;
			B = rand() % 19 - 9;
			C = rand() % 19 - 9;
		} while (A == 0 || B == 0 || C == 0 || A == B || A == C || B == C || fabs(A+B+C) <= 1.0f || fabs(A*C+B*C+A*B) <= 1.0f ||
			(int)fabs(A*B*C) <= 100);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve the equation.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(x^{3}%c%dx^{2}%c%dx%c%d=0\\)</p><p>\\(x\\ =\\ ?\\)</p>",
			(A+B+C>0?'+':'-'), (int)fabs(A+B+C), (A*C+B*C+A*B>0?'+':'-'), (int)fabs(A*C+B*C+A*B), 
			(A*B*C>0?'+':'-'), (int)fabs(A*B*C)); pos += bytes;
		bytes = sprintf(content + pos, "<p>Separate answers with commas.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%d,%d,%d\t", -A, -B, -C); pos += bytes;
		bytes = sprintf(answers + pos,  "%d,%d,%d\t", -A, -C, -B); pos += bytes;
		bytes = sprintf(answers + pos,  "%d,%d,%d\t", -B, -A, -C); pos += bytes;
		bytes = sprintf(answers + pos,  "%d,%d,%d\t", -B, -C, -A); pos += bytes;
		bytes = sprintf(answers + pos,  "%d,%d,%d\t", -C, -A, -B); pos += bytes;
		bytes = sprintf(answers + pos,  "%d,%d,%d", -C, -B, -A); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;		


	return 1;
};

int question_3_15(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 39 - 19;
			B = rand() % 39 - 19;
			C = rand() % 39 - 19;
		} while (A == 0 || B == 0 || C == 0 || A == B || A == C || B == C || fabs(A+B+C) <= 1.0f || fabs(A*C+B*C+A*B) <= 1.0f ||
			(int)fabs(A*B*C) > 100);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve the equation.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(x^{3}%c%dx^{2}%c%dx%c%d=0\\)</p><p>\\(x\\ =\\ ?\\)</p>",
			(A+B+C>0?'+':'-'), (int)fabs(A+B+C), (A*C+B*C+A*B>0?'+':'-'), (int)fabs(A*C+B*C+A*B), 
			(A*B*C>0?'+':'-'), (int)fabs(A*B*C)); pos += bytes;
		bytes = sprintf(content + pos, "<p>Separate answers with commas.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%d,%d,%d\t", -A, -B, -C); pos += bytes;
		bytes = sprintf(answers + pos,  "%d,%d,%d\t", -A, -C, -B); pos += bytes;
		bytes = sprintf(answers + pos,  "%d,%d,%d\t", -B, -A, -C); pos += bytes;
		bytes = sprintf(answers + pos,  "%d,%d,%d\t", -B, -C, -A); pos += bytes;
		bytes = sprintf(answers + pos,  "%d,%d,%d\t", -C, -A, -B); pos += bytes;
		bytes = sprintf(answers + pos,  "%d,%d,%d", -C, -B, -A); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;		


	return 1;
};

int question_3_16(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 19 - 9;
			B = rand() % 19 - 9;
			C = rand() % 19 + 1;
		} while (A == 0 || B == 0 || C == 0 || C <= 1 || !isprime(C) || fabs(-A-2*B) <= 1.0f || fabs(B*B-C+2*A*B) <= 1.0f ||
			fabs(-A*B*B+A*C) >= 100);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve the equation.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(x^{3}%c%dx^{2}%c%dx%c%d=0\\)</p><p>\\(x\\ =\\ ?\\)</p>",
			(-A-2*B>0?'+':'-'), (int)fabs(-A-2*B), (B*B-C+2*A*B>0?'+':'-'), (int)fabs(B*B-C+2*A*B), 
			(-A*B*B+A*C>0?'+':'-'), (int)fabs(-A*B*B+A*C)); pos += bytes;
		bytes = sprintf(content + pos, "<p>Separate answers with commas.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%d,%d{\\pm}{\\sqrt{`%d`}}\t", A, B, C); pos += bytes;
		bytes = sprintf(answers + pos,  "%d{\\pm}{\\sqrt{`%d`}},%d\t", B, C, A); pos += bytes;
		bytes = sprintf(answers + pos,  "%d,%d+{\\sqrt{`%d`}},%d-{\\sqrt{`%d`}}\t", A, B, C, B, C); pos += bytes;
		bytes = sprintf(answers + pos,  "%d+{\\sqrt{`%d`}},%d-{\\sqrt{`%d`}},%d\t", B, C, B, C, A); pos += bytes;
		bytes = sprintf(answers + pos,  "%d,%d-{\\sqrt{`%d`}},%d+{\\sqrt{`%d`}}\t", A, B, C, B, C); pos += bytes;
		bytes = sprintf(answers + pos,  "%d-{\\sqrt{`%d`}},%d+{\\sqrt{`%d`}},%d\t", B, C, B, C, A); pos += bytes;
		bytes = sprintf(answers + pos,  "%d+{\\sqrt{`%d`}},%d,%d-{\\sqrt{`%d`}}\t", B, C, A, B, C); pos += bytes;
		bytes = sprintf(answers + pos,  "%d+{\\sqrt{`%d`}},%d,%d-{\\sqrt{`%d`}}\t", B, C, A, B, C); pos += bytes;
		bytes = sprintf(answers + pos,  "%d-{\\sqrt{`%d`}},%d,%d+{\\sqrt{`%d`}}\t", B, C, A, B, C); pos += bytes;
		bytes = sprintf(answers + pos,  "%d-{\\sqrt{`%d`}},%d,%d+{\\sqrt{`%d`}}", B, C, A, B, C); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;		
	
	return 1;
};

int question_3_17(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 8 + 2;
			B = rand() % 19 - 9;
			C = rand() % 19 - 9;
		}
		while (B == 0 || C == 0 || B == C);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Find the domain of the rational function.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(f(x) = \\dfrac{ %dx^2 }{ (x%c%d)(x%c%d) }\\)</p>",
			A, (-B>0?'+':'-'), (int)fabs(-B), (-C>0?'+':'-'), (int)fabs(-C)); pos += bytes;
		bytes = sprintf(content + pos, "<p>The domain of the function in interval notation is \\(=\\ \\ ?\\)</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		if (B < C)
		{
			bytes = sprintf(answers + pos,  "(-{\\infty},%d){\\cup}(%d,%d){\\cup}(%d,{\\infty})", B, B, C, C); pos += bytes;
		}
		else
		{
			bytes = sprintf(answers + pos,  "(-{\\infty},%d){\\cup}(%d,%d){\\cup}(%d,{\\infty})", C, C, B, B); pos += bytes;
		}

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_3_18(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 11 - 5;
			B = rand() % 11 - 5;
		} while (A == 0 || B == 0 || A >= B || A > 0 || B < 0 || fabs(A) <= 1.0f || fabs(B) <= 1.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Use the graph of the rational function to complete the following statement.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "@f(x) = 10*x/((x-%d)*(x-%d)) ; ", A, B); pos += bytes;
		bytes = sprintf(content + pos, "set for [i=1:9] linetype i dashtype i ; set arrow from %d,%d to %d,%d nohead lc rgb '#009e73' lw 2 dt '-' ; set arrow from %d,%d to %d,%d nohead lc rgb '#009e73' lw 2 dt '-' ; ", 
			A,-10,A,10, B,-10,B,10); pos += bytes;
		bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
		bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
		bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;

		if (randomizer% 4 == 0)
		{		
			bytes = sprintf(content + pos, "<p>As \\(x \\rightarrow %d^{-}, f(x) \\rightarrow \\ ?\\)</p>", A); pos += bytes;
		}
		else if (randomizer% 4 == 1)
		{
			bytes = sprintf(content + pos, "<p>As \\(x \\rightarrow %d^{+}, f(x) \\rightarrow \\ ?\\)</p>", A); pos += bytes;
		}
		else if (randomizer% 4 == 2)
		{		
			bytes = sprintf(content + pos, "<p>As \\(x \\rightarrow %d^{-}, f(x) \\rightarrow \\ ?\\)</p>", B); pos += bytes;
		}
		else if (randomizer% 4 == 3)
		{
			bytes = sprintf(content + pos, "<p>As \\(x \\rightarrow %d^{+}, f(x) \\rightarrow \\ ?\\)</p>", B); pos += bytes;
		}

		bytes = 0;
		pos = 0;			

		if (randomizer% 2 == 0)
		{
			bytes = sprintf(answers + pos,  "-{\\infty}"); pos += bytes;
		}
		else
		{
			bytes = sprintf(answers + pos,  "{\\infty}"); pos += bytes;
		}

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_3_19(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 19 - 9;
			B = rand() % 19 - 9;
		}
		while (A == 0 || B == 0 || A == B || fabs(A+B) <= 1.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		if (randomizer% 2 == 0)
		{
			bytes = sprintf(content + pos, "<p>Find the vertical asymptote of the rational function.</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>\\(f(x) = \\dfrac{ x%c%d }{ x^{2}%c%dx%c%d }\\)</p>",
				(A>0?'+':'-'), (int)fabs(A), (A+B>0?'+':'-'), (int)fabs(A+B), (A*B>0?'+':'-'), (int)fabs(A*B)); pos += bytes;
			bytes = sprintf(content + pos, "<p>The vertical asymptote is at \\(\\ ?\\)</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>Type an equation.</p>"); pos += bytes;	

			bytes = 0;
			pos = 0;			
	
			bytes = sprintf(answers + pos,  "x=%d", -B); pos += bytes;
		}
		else if (randomizer% 2 == 1)
		{
			bytes = sprintf(content + pos, "<p>Find the hole of the rational function.</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>\\(f(x) = \\dfrac{ x%c%d }{ x^{2}%c%dx%c%d }\\)</p>",
				(A>0?'+':'-'), (int)fabs(A), (A+B>0?'+':'-'), (int)fabs(A+B), (A*B>0?'+':'-'), (int)fabs(A*B)); pos += bytes;
			bytes = sprintf(content + pos, "<p>The hole is at \\(\\ ?\\)</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>Type an equation.</p>"); pos += bytes;	

			bytes = 0;
			pos = 0;			
	
			bytes = sprintf(answers + pos,  "x=%d", -A); pos += bytes;
		}

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_3_20(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 19 - 9;
			B = rand() % 19 - 9;
			C = rand() % 19 - 9;
			D = rand() % 18 + 2;
			E = rand() % 18 + 2;
		}
		while (A == 0 || B == 0 || C == 0 || A == B || fabs(A+B) <= 1.0f || C == A || C == B || D == E || !isprime(D) || !isprime(E));

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		if (randomizer% 2 == 0)
		{
			bytes = sprintf(content + pos, "<p>Find the horizontal asymptote of the rational function.</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>\\( f(x) = \\dfrac{ %dx%c%d }{ %dx^{2}%c%dx%c%d } \\)</p>",
				D, (C>0?'+':'-'), (int)fabs(C), E, (A+B>0?'+':'-'), (int)fabs(A+B), (A*B>0?'+':'-'), (int)fabs(A*B)); pos += bytes;
			bytes = sprintf(content + pos, "<p>The horizontal asymptote is at \\(\\ ?\\)</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>Type an equation.</p>"); pos += bytes;	

			bytes = 0;
			pos = 0;			
	
			bytes = sprintf(answers + pos,  "y=0"); pos += bytes;
		}
		else if (randomizer% 2 == 1)
		{
			bytes = sprintf(content + pos, "<p>Find the horizontal asymptote of the rational function.</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>\\( f(x) = \\dfrac{ %d x^{2}%c%d }{ %d x^{2}%c%dx%c%d } \\)</p>",
				D, (C>0?'+':'-'), (int)fabs(C), E, (A+B>0?'+':'-'), (int)fabs(A+B), (A*B>0?'+':'-'), (int)fabs(A*B)); pos += bytes;
			bytes = sprintf(content + pos, "<p>The horizontal asymptote is at \\(\\ ?\\)</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>Type an equation.</p>"); pos += bytes;	

			bytes = 0;
			pos = 0;			
	
			bytes = sprintf(answers + pos,  "y={\\dfrac{`%d`}{`%d`}}", D, E); pos += bytes;
		}

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_3_21(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 19 - 9;
			B = rand() % 19 - 9;
			C = rand() % 19 - 9;
			D = rand() % 18 + 2;
			E = rand() % 18 + 2;
		}
		while (A == 0 || B == 0 || C == 0 || A == B || fabs(A+B) <= 1.0f || C == A || C == B || D == E || !isprime(D) || !isprime(E));

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		if (randomizer% 3 == 0)
		{
			bytes = sprintf(content + pos, "<p>Does the rational function have a horizontal asymptote?</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>\\( f(x) = \\dfrac{ %dx%c%d }{ %dx^{2}%c%dx%c%d } \\)</p>",
				D, (C>0?'+':'-'), (int)fabs(C), E, (A+B>0?'+':'-'), (int)fabs(A+B), (A*B>0?'+':'-'), (int)fabs(A*B)); pos += bytes;
			bytes = sprintf(content + pos, "<p>a) Yes</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>b) No</p>"); pos += bytes;	

			bytes = 0;
			pos = 0;			
	
			bytes = sprintf(answers + pos,  "{a}"); pos += bytes;
		}
		else if (randomizer% 3 == 1)
		{
			bytes = sprintf(content + pos, "<p>Does the rational function have a horizontal asymptote?</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>\\( f(x) = \\dfrac{ %dx^{2}%c%d }{ %dx^{2}%c%dx%c%d } \\)</p>",
				D, (C>0?'+':'-'), (int)fabs(C), E, (A+B>0?'+':'-'), (int)fabs(A+B), (A*B>0?'+':'-'), (int)fabs(A*B)); pos += bytes;
			bytes = sprintf(content + pos, "<p>a) Yes</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>b) No</p>"); pos += bytes;	

			bytes = 0;
			pos = 0;			
	
			bytes = sprintf(answers + pos,  "{a}"); pos += bytes;
		}
		else if (randomizer% 3 == 2)
		{
			bytes = sprintf(content + pos, "<p>Does the rational function have a horizontal asymptote?</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>\\( f(x) = \\dfrac{ %dx^{3}%c%d }{ %dx^{2}%c%dx%c%d } \\)</p>",
				D, (C>0?'+':'-'), (int)fabs(C), E, (A+B>0?'+':'-'), (int)fabs(A+B), (A*B>0?'+':'-'), (int)fabs(A*B)); pos += bytes;
			bytes = sprintf(content + pos, "<p>a) Yes</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>b) No</p>"); pos += bytes;	

			bytes = 0;
			pos = 0;			
	
			bytes = sprintf(answers + pos,  "{b}"); pos += bytes;
		}

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_3_22(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 19 - 9;
			B = rand() % 19 - 9;
			C = rand() % 19 - 9;
		}
		while (A == 0 || B == 0 || C == 0 || A == B || fabs(A+B) <= 1.0f || C == A || C == B);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Find the equations of the vertical asymptotes of the rational function.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\( f(x) = \\dfrac{ x%c%d }{ x^{2}%c%dx%c%d } \\)</p>",
			(C>0?'+':'-'), (int)fabs(C), (A+B>0?'+':'-'), (int)fabs(A+B), (A*B>0?'+':'-'), (int)fabs(A*B)); pos += bytes;
		bytes = sprintf(content + pos, "<p>The vertical asymptotes are at \\(\\ ?\\)</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>Type equations separated by commas.</p>"); pos += bytes;	

		bytes = 0;
		pos = 0;			
	
		bytes = sprintf(answers + pos,  "x=%d,x=%d\t", -A, -B); pos += bytes;
		bytes = sprintf(answers + pos,  "x=%d,x=%d", -B, -A); pos += bytes;
		
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_3_23(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 19 - 9;
			B = rand() % 19 - 9;
			C = rand() % 19 - 9;
		}
		while (A == 0 || B == 0 || C == 0 || A == B || fabs(A+B) <= 1.0f || A > B || C == A || C == B || C < A || C > B);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Pick the graph of the rational function.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\( f(x) = \\dfrac{ x%c%d }{ x^{2}%c%dx%c%d } \\)</p>",
			(C>0?'+':'-'), (int)fabs(C), (A+B>0?'+':'-'), (int)fabs(A+B), (A*B>0?'+':'-'), (int)fabs(A*B)); pos += bytes;

		if (randomizer% 4 == 0)
		{
			bytes = sprintf(content + pos, "<p style='border:1px solid black'>a)"); pos += bytes;
			bytes = sprintf(content + pos, "@f(x) = 10*(x+%d)/((x+%d)*(x+%d)) ; ", C, A, B); pos += bytes;
			bytes = sprintf(content + pos, "set for [i=1:9] linetype i dashtype i ; set arrow from %d,%d to %d,%d nohead lc rgb '#009e73' lw 2 dt '-' ; set arrow from %d,%d to %d,%d nohead lc rgb '#009e73' lw 2 dt '-' ; ", 
				-A,-10,-A,10, -B,-10,-B,10); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos, "</p>"); pos += bytes;
	
			bytes = sprintf(content + pos, "<p style='border:1px solid black'>b)"); pos += bytes;
			bytes = sprintf(content + pos, "@f(x) = 10*(x-%d)/((x-%d)*(x-%d)) ; ", C, A, B); pos += bytes;
			bytes = sprintf(content + pos, "set for [i=1:9] linetype i dashtype i ; set arrow from %d,%d to %d,%d nohead lc rgb '#009e73' lw 2 dt '-' ; set arrow from %d,%d to %d,%d nohead lc rgb '#009e73' lw 2 dt '-' ; ", 
				A,-10,A,10, B,-10,B,10); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos, "</p>"); pos += bytes;
	
			bytes = sprintf(content + pos, "<p style='border:1px solid black'>c)"); pos += bytes;
			bytes = sprintf(content + pos, "@f(x) = -10*(x+%d)/((x+%d)*(x+%d)) ; ", C, A, B); pos += bytes;
			bytes = sprintf(content + pos, "set for [i=1:9] linetype i dashtype i ; set arrow from %d,%d to %d,%d nohead lc rgb '#009e73' lw 2 dt '-' ; set arrow from %d,%d to %d,%d nohead lc rgb '#009e73' lw 2 dt '-' ; ", 
				-A,-10,-A,10, -B,-10,-B,10); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos, "</p>"); pos += bytes;
	
			bytes = sprintf(content + pos, "<p style='border:1px solid black'>d)"); pos += bytes;
			bytes = sprintf(content + pos, "@f(x) = -10*(x-%d)/((x-%d)*(x-%d)) ; ", C, A, B); pos += bytes;
			bytes = sprintf(content + pos, "set for [i=1:9] linetype i dashtype i ; set arrow from %d,%d to %d,%d nohead lc rgb '#009e73' lw 2 dt '-' ; set arrow from %d,%d to %d,%d nohead lc rgb '#009e73' lw 2 dt '-' ; ", 
				A,-10,A,10, B,-10,B,10); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos, "</p>"); pos += bytes;

			bytes = 0;
			pos = 0;			
		
			bytes = sprintf(answers + pos,  "{a}"); pos += bytes;
		}
		else if (randomizer% 4 == 1)
		{
			bytes = sprintf(content + pos, "<p style='border:1px solid black'>a)"); pos += bytes;
			bytes = sprintf(content + pos, "@f(x) = 10*(x-%d)/((x-%d)*(x-%d)) ; ", C, A, B); pos += bytes;
			bytes = sprintf(content + pos, "set for [i=1:9] linetype i dashtype i ; set arrow from %d,%d to %d,%d nohead lc rgb '#009e73' lw 2 dt '-' ; set arrow from %d,%d to %d,%d nohead lc rgb '#009e73' lw 2 dt '-' ; ", 
				A,-10,A,10, B,-10,B,10); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos, "</p>"); pos += bytes;

			bytes = sprintf(content + pos, "<p style='border:1px solid black'>b)"); pos += bytes;
			bytes = sprintf(content + pos, "@f(x) = 10*(x+%d)/((x+%d)*(x+%d)) ; ", C, A, B); pos += bytes;
			bytes = sprintf(content + pos, "set for [i=1:9] linetype i dashtype i ; set arrow from %d,%d to %d,%d nohead lc rgb '#009e73' lw 2 dt '-' ; set arrow from %d,%d to %d,%d nohead lc rgb '#009e73' lw 2 dt '-' ; ", 
				-A,-10,-A,10, -B,-10,-B,10); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos, "</p>"); pos += bytes;
	
			bytes = sprintf(content + pos, "<p style='border:1px solid black'>c)"); pos += bytes;
			bytes = sprintf(content + pos, "@f(x) = -10*(x-%d)/((x-%d)*(x-%d)) ; ", C, A, B); pos += bytes;
			bytes = sprintf(content + pos, "set for [i=1:9] linetype i dashtype i ; set arrow from %d,%d to %d,%d nohead lc rgb '#009e73' lw 2 dt '-' ; set arrow from %d,%d to %d,%d nohead lc rgb '#009e73' lw 2 dt '-' ; ", 
				A,-10,A,10, B,-10,B,10); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos, "</p>"); pos += bytes;

			bytes = sprintf(content + pos, "<p style='border:1px solid black'>d)"); pos += bytes;
			bytes = sprintf(content + pos, "@f(x) = -10*(x+%d)/((x+%d)*(x+%d)) ; ", C, A, B); pos += bytes;
			bytes = sprintf(content + pos, "set for [i=1:9] linetype i dashtype i ; set arrow from %d,%d to %d,%d nohead lc rgb '#009e73' lw 2 dt '-' ; set arrow from %d,%d to %d,%d nohead lc rgb '#009e73' lw 2 dt '-' ; ", 
				-A,-10,-A,10, -B,-10,-B,10); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos, "</p>"); pos += bytes;

			bytes = 0;
			pos = 0;			
		
			bytes = sprintf(answers + pos,  "{b}"); pos += bytes;
		}
		else if (randomizer% 4 == 2)
		{
			bytes = sprintf(content + pos, "<p style='border:1px solid black'>a)"); pos += bytes;
			bytes = sprintf(content + pos, "@f(x) = -10*(x+%d)/((x+%d)*(x+%d)) ; ", C, A, B); pos += bytes;
			bytes = sprintf(content + pos, "set for [i=1:9] linetype i dashtype i ; set arrow from %d,%d to %d,%d nohead lc rgb '#009e73' lw 2 dt '-' ; set arrow from %d,%d to %d,%d nohead lc rgb '#009e73' lw 2 dt '-' ; ", 
				-A,-10,-A,10, -B,-10,-B,10); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos, "</p>"); pos += bytes;

			bytes = sprintf(content + pos, "<p style='border:1px solid black'>b)"); pos += bytes;
			bytes = sprintf(content + pos, "@f(x) = 10*(x-%d)/((x-%d)*(x-%d)) ; ", C, A, B); pos += bytes;
			bytes = sprintf(content + pos, "set for [i=1:9] linetype i dashtype i ; set arrow from %d,%d to %d,%d nohead lc rgb '#009e73' lw 2 dt '-' ; set arrow from %d,%d to %d,%d nohead lc rgb '#009e73' lw 2 dt '-' ; ", 
				A,-10,A,10, B,-10,B,10); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos, "</p>"); pos += bytes;

			bytes = sprintf(content + pos, "<p style='border:1px solid black'>c)"); pos += bytes;
			bytes = sprintf(content + pos, "@f(x) = 10*(x+%d)/((x+%d)*(x+%d)) ; ", C, A, B); pos += bytes;
			bytes = sprintf(content + pos, "set for [i=1:9] linetype i dashtype i ; set arrow from %d,%d to %d,%d nohead lc rgb '#009e73' lw 2 dt '-' ; set arrow from %d,%d to %d,%d nohead lc rgb '#009e73' lw 2 dt '-' ; ", 
				-A,-10,-A,10, -B,-10,-B,10); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos, "</p>"); pos += bytes;
	
			bytes = sprintf(content + pos, "<p style='border:1px solid black'>d)"); pos += bytes;
			bytes = sprintf(content + pos, "@f(x) = -10*(x-%d)/((x-%d)*(x-%d)) ; ", C, A, B); pos += bytes;
			bytes = sprintf(content + pos, "set for [i=1:9] linetype i dashtype i ; set arrow from %d,%d to %d,%d nohead lc rgb '#009e73' lw 2 dt '-' ; set arrow from %d,%d to %d,%d nohead lc rgb '#009e73' lw 2 dt '-' ; ", 
				A,-10,A,10, B,-10,B,10); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos, "</p>"); pos += bytes;

			bytes = 0;
			pos = 0;			
		
			bytes = sprintf(answers + pos,  "{c}"); pos += bytes;
		}
		else if (randomizer% 4 == 3)
		{
			bytes = sprintf(content + pos, "<p style='border:1px solid black'>a)"); pos += bytes;
			bytes = sprintf(content + pos, "@f(x) = 10*(x-%d)/((x-%d)*(x-%d)) ; ", C, A, B); pos += bytes;
			bytes = sprintf(content + pos, "set for [i=1:9] linetype i dashtype i ; set arrow from %d,%d to %d,%d nohead lc rgb '#009e73' lw 2 dt '-' ; set arrow from %d,%d to %d,%d nohead lc rgb '#009e73' lw 2 dt '-' ; ", 
				A,-10,A,10, B,-10,B,10); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos, "</p>"); pos += bytes;

			bytes = sprintf(content + pos, "<p style='border:1px solid black'>b)"); pos += bytes;
			bytes = sprintf(content + pos, "@f(x) = -10*(x+%d)/((x+%d)*(x+%d)) ; ", C, A, B); pos += bytes;
			bytes = sprintf(content + pos, "set for [i=1:9] linetype i dashtype i ; set arrow from %d,%d to %d,%d nohead lc rgb '#009e73' lw 2 dt '-' ; set arrow from %d,%d to %d,%d nohead lc rgb '#009e73' lw 2 dt '-' ; ", 
				-A,-10,-A,10, -B,-10,-B,10); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos, "</p>"); pos += bytes;
	
			bytes = sprintf(content + pos, "<p style='border:1px solid black'>c)"); pos += bytes;
			bytes = sprintf(content + pos, "@f(x) = -10*(x-%d)/((x-%d)*(x-%d)) ; ", C, A, B); pos += bytes;
			bytes = sprintf(content + pos, "set for [i=1:9] linetype i dashtype i ; set arrow from %d,%d to %d,%d nohead lc rgb '#009e73' lw 2 dt '-' ; set arrow from %d,%d to %d,%d nohead lc rgb '#009e73' lw 2 dt '-' ; ", 
				A,-10,A,10, B,-10,B,10); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos, "</p>"); pos += bytes;
			bytes = 0;
			pos = 0;

			bytes = sprintf(content + pos, "<p style='border:1px solid black'>d)"); pos += bytes;
			bytes = sprintf(content + pos, "@f(x) = 10*(x+%d)/((x+%d)*(x+%d)) ; ", C, A, B); pos += bytes;
			bytes = sprintf(content + pos, "set for [i=1:9] linetype i dashtype i ; set arrow from %d,%d to %d,%d nohead lc rgb '#009e73' lw 2 dt '-' ; set arrow from %d,%d to %d,%d nohead lc rgb '#009e73' lw 2 dt '-' ; ", 
				-A,-10,-A,10, -B,-10,-B,10); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos, "</p>"); pos += bytes;			
		
			bytes = sprintf(answers + pos,  "{d}"); pos += bytes;
		}
		
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;	

	return 1;
};

int question_3_24(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 10 - 4;
			B = rand() % 10 - 4;
		}
		while (A == 0 || B == 0 || A == B || fabs(A-B) <= 1.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Find the rational function of the given graph.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>Hint: The coefficients for both the numerator and denominator are 1.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "@f(x) = (x+%d)/(x+%d) ; ", A, B); pos += bytes;
		bytes = sprintf(content + pos, "set for [i=1:9] linetype i dashtype i ; set arrow from %d,%d to %d,%d nohead lc rgb '#009e73' lw 2 dt '-' ; ", 
			-B,-10,-B,10); pos += bytes;
		bytes = sprintf(content + pos, "set for [i=1:9] linetype i dashtype i ; set arrow from %d,%d to %d,%d nohead lc rgb '#009e73' lw 2 dt '-' ; ", 
			-10,1,10,1); pos += bytes;
		bytes = sprintf(content + pos, "set object 1 circle at %d,%d size 0.333 fillcolor rgb 'dark-violet' fill solid noborder; ", -A, 0); pos += bytes;
		bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
		bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
		bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
		bytes = sprintf(content + pos, "<p>The rational function is \\(f(x) =\\ ?\\)</p>"); pos += bytes;
			

		bytes = 0;
		pos = 0;			
	
		bytes = sprintf(answers + pos,  "{\\dfrac{`x%c%d`}{`x%c%d`}}\t", (A>0?'+':'-'), (int)fabs(A), (B>0?'+':'-'), (int)fabs(B)); pos += bytes;
		bytes = sprintf(answers + pos,  "{\\dfrac{`(x%c%d)`}{`(x%c%d)`}}\t", (A>0?'+':'-'), (int)fabs(A), (B>0?'+':'-'), (int)fabs(B)); pos += bytes;
		bytes = sprintf(answers + pos,  "{\\dfrac{`1x%c%d`}{`1x%c%d`}}\t", (A>0?'+':'-'), (int)fabs(A), (B>0?'+':'-'), (int)fabs(B)); pos += bytes;
		bytes = sprintf(answers + pos,  "{\\dfrac{`(1x%c%d)`}{`(1x%c%d)`}}\t", (A>0?'+':'-'), (int)fabs(A), (B>0?'+':'-'), (int)fabs(B)); pos += bytes;
		bytes = sprintf(answers + pos,  "{\\dfrac{`1(x%c%d)`}{`1(x%c%d)`}}\t", (A>0?'+':'-'), (int)fabs(A), (B>0?'+':'-'), (int)fabs(B)); pos += bytes;

		bytes = sprintf(answers + pos,  "{\\dfrac{`%d+x`}{`x%c%d`}}\t", A, (B>0?'+':'-'), (int)fabs(B)); pos += bytes;
		bytes = sprintf(answers + pos,  "{\\dfrac{`(%d+x)`}{`(x%c%d)`}}\t", A, (B>0?'+':'-'), (int)fabs(B)); pos += bytes;
		bytes = sprintf(answers + pos,  "{\\dfrac{`%d+1x`}{`1x%c%d`}}\t", A, (B>0?'+':'-'), (int)fabs(B)); pos += bytes;
		bytes = sprintf(answers + pos,  "{\\dfrac{`(%d+1x)`}{`(1x%c%d)`}}\t", A, (B>0?'+':'-'), (int)fabs(B)); pos += bytes;
		bytes = sprintf(answers + pos,  "{\\dfrac{`1(%d+x)`}{`1(x%c%d)`}}\t", A, (B>0?'+':'-'), (int)fabs(B)); pos += bytes;

		bytes = sprintf(answers + pos,  "{\\dfrac{`x%c%d`}{`%d+x`}}\t", (A>0?'+':'-'), (int)fabs(A), B); pos += bytes;
		bytes = sprintf(answers + pos,  "{\\dfrac{`(x%c%d)`}{`(%d+x)`}}\t", (A>0?'+':'-'), (int)fabs(A), B); pos += bytes;
		bytes = sprintf(answers + pos,  "{\\dfrac{`1x%c%d`}{`%d+1x`}}\t", (A>0?'+':'-'), (int)fabs(A), B); pos += bytes;
		bytes = sprintf(answers + pos,  "{\\dfrac{`(1x%c%d)`}{`(%d+1x)`}}\t", (A>0?'+':'-'), (int)fabs(A), B); pos += bytes;
		bytes = sprintf(answers + pos,  "{\\dfrac{`1(x%c%d)`}{`1(%d+x)`}}\t", (A>0?'+':'-'), (int)fabs(A), B); pos += bytes;

		bytes = sprintf(answers + pos,  "{\\dfrac{`%d+x`}{`%d+x`}}\t", A, B); pos += bytes;
		bytes = sprintf(answers + pos,  "{\\dfrac{`(%d+x)`}{`(%d+x)`}}\t", A, B); pos += bytes;
		bytes = sprintf(answers + pos,  "{\\dfrac{`%d+1x`}{`%d+1x`}}\t", A, B); pos += bytes;
		bytes = sprintf(answers + pos,  "{\\dfrac{`(%d+1x)`}{`(%d+1x)`}}\t", A, B); pos += bytes;
		bytes = sprintf(answers + pos,  "{\\dfrac{`1(%d+x)`}{`1(%d+x)`}}", A, B); pos += bytes;				

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;
	
	return 1;
};

int question_3_25(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 19 - 9;
			B = rand() % 19 - 9;
		}
		while (A == 0 || B == 0 || A == B || fabs(A+B) <= 1.0f || A <= B || fabs(A-B) <= 2.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		if (randomizer% 4 == 0)
		{
			bytes = 0;
			pos = 0;

			bytes = sprintf(content + pos, "<p>Solve the polynomial inequality.</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>\\(x^{2}%c%dx%c%d\\gt 0\\)</p>",
				(A+B>0?'+':'-'), (int)fabs(A+B), (A*B>0?'+':'-'), (int)fabs(A*B)); pos += bytes;
			bytes = sprintf(content + pos, "<p>The solution set is \\(=\\ \\ ?\\)</p>"); pos += bytes;
	
			bytes = 0;
			pos = 0;			
	
			bytes = sprintf(answers + pos,  "(-{\\infty},%d){\\cup}(%d,{\\infty})", -A, -B); pos += bytes;
		}
		else if (randomizer% 4 == 1)
		{
			bytes = 0;
			pos = 0;

			bytes = sprintf(content + pos, "<p>Solve the polynomial inequality.</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>\\(x^{2}%c%dx%c%d\\lt 0\\)</p>",
				(A+B>0?'+':'-'), (int)fabs(A+B), (A*B>0?'+':'-'), (int)fabs(A*B)); pos += bytes;
			bytes = sprintf(content + pos, "<p>The solution set is \\(=\\ \\ ?\\)</p>"); pos += bytes;
	
			bytes = 0;
			pos = 0;			
	
			bytes = sprintf(answers + pos,  "(%d,%d)", -A, -B); pos += bytes;
		}
		else if (randomizer% 4 == 2)
		{
			bytes = 0;
			pos = 0;

			bytes = sprintf(content + pos, "<p>Solve the polynomial inequality.</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>\\(x^{2}%c%dx%c%d\\ge 0\\)</p>",
				(A+B>0?'+':'-'), (int)fabs(A+B), (A*B>0?'+':'-'), (int)fabs(A*B)); pos += bytes;
			bytes = sprintf(content + pos, "<p>The solution set is \\(=\\ \\ ?\\)</p>"); pos += bytes;
	
			bytes = 0;
			pos = 0;			
	
			bytes = sprintf(answers + pos,  "(-{\\infty},%d]{\\cup}[%d,{\\infty})", -A, -B); pos += bytes;
		}
		else if (randomizer% 4 == 3)
		{
			bytes = 0;
			pos = 0;

			bytes = sprintf(content + pos, "<p>Solve the polynomial inequality.</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>\\(x^{2}%c%dx%c%d\\le 0\\)</p>",
				(A+B>0?'+':'-'), (int)fabs(A+B), (A*B>0?'+':'-'), (int)fabs(A*B)); pos += bytes;
			bytes = sprintf(content + pos, "<p>The solution set is \\(=\\ \\ ?\\)</p>"); pos += bytes;
	
			bytes = 0;
			pos = 0;			
	
			bytes = sprintf(answers + pos,  "[%d,%d]", -A, -B); pos += bytes;
		}

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_3_26(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 39 - 19;
		}
		while (A == 0 || fabs(A) <= 1.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		if (randomizer% 4 == 0)
		{
			bytes = 0;
			pos = 0;

			bytes = sprintf(content + pos, "<p>Solve the polynomial inequality.</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>\\(x^{2}%c%dx\\gt 0\\)</p>",
				(A>0?'+':'-'), (int)fabs(A)); pos += bytes;
			bytes = sprintf(content + pos, "<p>The solution set is \\(=\\ \\ ?\\)</p>"); pos += bytes;
	
			bytes = 0;
			pos = 0;			
	
			if (A > 0)
			{
				bytes = sprintf(answers + pos,  "(-{\\infty},%d){\\cup}(0,{\\infty})", -A); pos += bytes;
			}
			else
			{
				bytes = sprintf(answers + pos,  "(-{\\infty},0){\\cup}(%d,{\\infty})", -A); pos += bytes;
			}
		}
		else if (randomizer% 4 == 1)
		{
			bytes = 0;
			pos = 0;

			bytes = sprintf(content + pos, "<p>Solve the polynomial inequality.</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>\\(x^{2}%c%dx\\lt 0\\)</p>",
				(A>0?'+':'-'), (int)fabs(A)); pos += bytes;
			bytes = sprintf(content + pos, "<p>The solution set is \\(=\\ \\ ?\\)</p>"); pos += bytes;
	
			bytes = 0;
			pos = 0;			
	
			if (A > 0)
			{
				bytes = sprintf(answers + pos,  "(%d,0)", -A); pos += bytes;
			}
			else
			{
				bytes = sprintf(answers + pos,  "(0,%d)", -A); pos += bytes;
			}
		}
		else if (randomizer% 4 == 2)
		{
			bytes = 0;
			pos = 0;

			bytes = sprintf(content + pos, "<p>Solve the polynomial inequality.</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>\\(x^{2}%c%dx\\ge 0\\)</p>",
				(A>0?'+':'-'), (int)fabs(A)); pos += bytes;
			bytes = sprintf(content + pos, "<p>The solution set is \\(=\\ \\ ?\\)</p>"); pos += bytes;
	
			bytes = 0;
			pos = 0;			
	
			if (A > 0)
			{
				bytes = sprintf(answers + pos,  "(-{\\infty},%d]{\\cup}[0,{\\infty})", -A); pos += bytes;
			}
			else
			{
				bytes = sprintf(answers + pos,  "(-{\\infty},0]{\\cup}[%d,{\\infty})", -A); pos += bytes;
			}
		}
		else if (randomizer% 4 == 3)
		{
			bytes = 0;
			pos = 0;

			bytes = sprintf(content + pos, "<p>Solve the polynomial inequality.</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>\\(x^{2}%c%dx\\le 0\\)</p>",
				(A>0?'+':'-'), (int)fabs(A)); pos += bytes;
			bytes = sprintf(content + pos, "<p>The solution set is \\(=\\ \\ ?\\)</p>"); pos += bytes;
	
			bytes = 0;
			pos = 0;			
	
			if (A > 0)
			{
				bytes = sprintf(answers + pos,  "[%d,0]", -A); pos += bytes;
			}
			else
			{
				bytes = sprintf(answers + pos,  "[0,%d]", -A); pos += bytes;
			}
		}

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_3_27(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 18 + 2;
			B = rand() % 39 - 19;
		}
		while (A == 0 || B == 0 || fabs(B) <= 1.0f || A == B || A == -B || !isprime(A) || !isprime((int)fabs(B)));

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		if (randomizer% 4 == 0)
		{
			bytes = 0;
			pos = 0;

			bytes = sprintf(content + pos, "<p>Solve the polynomial inequality.</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>\\(%dx^{2}%c%dx\\gt 0\\)</p>",
				A, (B>0?'+':'-'), (int)fabs(B)); pos += bytes;
			bytes = sprintf(content + pos, "<p>The solution set is \\(=\\ \\ ?\\)</p>"); pos += bytes;
	
			bytes = 0;
			pos = 0;			
	
			if (B > 0)
			{
				bytes = sprintf(answers + pos,  "(-{\\infty},{\\dfrac{`-%d`}{`%d`}}){\\cup}(0,{\\infty})\t", (int)fabs(B), A); pos += bytes;
				bytes = sprintf(answers + pos,  "(-{\\infty},{\\dfrac{`%d`}{`-%d`}}){\\cup}(0,{\\infty})\t", (int)fabs(B), A); pos += bytes;
				bytes = sprintf(answers + pos,  "(-{\\infty},-{\\dfrac{`%d`}{`%d`}}){\\cup}(0,{\\infty})", (int)fabs(B), A); pos += bytes;
			}
			else
			{
				bytes = sprintf(answers + pos,  "(-{\\infty},0){\\cup}({\\dfrac{`%d`}{`%d`}},{\\infty})", (int)fabs(B), A); pos += bytes;
			}
		}
		else if (randomizer% 4 == 1)
		{
			bytes = 0;
			pos = 0;

			bytes = sprintf(content + pos, "<p>Solve the polynomial inequality.</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>\\(%dx^{2}%c%dx\\lt 0\\)</p>",
				A, (B>0?'+':'-'), (int)fabs(B)); pos += bytes;
			bytes = sprintf(content + pos, "<p>The solution set is \\(=\\ \\ ?\\)</p>"); pos += bytes;
	
			bytes = 0;
			pos = 0;			
	
			if (B > 0)
			{
				bytes = sprintf(answers + pos,  "({\\dfrac{`-%d`}{`%d`}},0)\t", (int)fabs(B), A); pos += bytes;
				bytes = sprintf(answers + pos,  "({\\dfrac{`%d`}{`-%d`}},0)\t", (int)fabs(B), A); pos += bytes;
				bytes = sprintf(answers + pos,  "(-{\\dfrac{`%d`}{`%d`}},0)", (int)fabs(B), A); pos += bytes;
			}
			else
			{
				bytes = sprintf(answers + pos,  "(0,{\\dfrac{`%d`}{`%d`}})", (int)fabs(B), A); pos += bytes;
			}
		}
		else if (randomizer% 4 == 2)
		{
			bytes = 0;
			pos = 0;

			bytes = sprintf(content + pos, "<p>Solve the polynomial inequality.</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>\\(%dx^{2}%c%dx\\ge 0\\)</p>",
				A, (B>0?'+':'-'), (int)fabs(B)); pos += bytes;
			bytes = sprintf(content + pos, "<p>The solution set is \\(=\\ \\ ?\\)</p>"); pos += bytes;
	
			bytes = 0;
			pos = 0;			
	
			if (B > 0)
			{
				bytes = sprintf(answers + pos,  "(-{\\infty},{\\dfrac{`-%d`}{`%d`}}]{\\cup}[0,{\\infty})\t", (int)fabs(B), A); pos += bytes;
				bytes = sprintf(answers + pos,  "(-{\\infty},{\\dfrac{`%d`}{`-%d`}}]{\\cup}[0,{\\infty})\t", (int)fabs(B), A); pos += bytes;
				bytes = sprintf(answers + pos,  "(-{\\infty},-{\\dfrac{`%d`}{`%d`}}]{\\cup}[0,{\\infty})\t", (int)fabs(B), A); pos += bytes;
			}
			else
			{
				bytes = sprintf(answers + pos,  "(-{\\infty},0]{\\cup}[{\\dfrac{`%d`}{`%d`}},{\\infty})", (int)fabs(B), A); pos += bytes;
			}
		}
		else if (randomizer% 4 == 3)
		{
			bytes = 0;
			pos = 0;

			bytes = sprintf(content + pos, "<p>Solve the polynomial inequality.</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>\\(%dx^{2}%c%dx\\le 0\\)</p>",
				A, (B>0?'+':'-'), (int)fabs(B)); pos += bytes;
			bytes = sprintf(content + pos, "<p>The solution set is \\(=\\ \\ ?\\)</p>"); pos += bytes;
	
			bytes = 0;
			pos = 0;			
	
			if (B > 0)
			{
				bytes = sprintf(answers + pos,  "[{\\dfrac{`-%d`}{`%d`}},0]\t", (int)fabs(B), A); pos += bytes;
				bytes = sprintf(answers + pos,  "[{\\dfrac{`%d`}{`-%d`}},0]\t", (int)fabs(B), A); pos += bytes;
				bytes = sprintf(answers + pos,  "[-{\\dfrac{`%d`}{`%d`}},0]", (int)fabs(B), A); pos += bytes;
			}
			else
			{
				bytes = sprintf(answers + pos,  "[0,{\\dfrac{`%d`}{`%d`}}]", (int)fabs(B), A); pos += bytes;
			}
		}

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_3_28(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 19 - 9;
			B = rand() % 19 - 9;
			C = rand() % 19 - 9;
		}
		while (A == 0 || B == 0 || C == 0 || A == B || A == C || B == C || A <= B || A <= C || B <= C);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		if (randomizer% 4 == 0)
		{
			bytes = 0;
			pos = 0;

			bytes = sprintf(content + pos, "<p>Solve the polynomial inequality.</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>\\((x%c%d)(x%c%d)(x%c%d)\\gt 0\\)</p>",
				(A>0?'+':'-'), (int)fabs(A), (B>0?'+':'-'), (int)fabs(B), (C>0?'+':'-'), (int)fabs(C)); pos += bytes;
			bytes = sprintf(content + pos, "<p>The solution set is \\(=\\ \\ ?\\)</p>"); pos += bytes;
	
			bytes = 0;
			pos = 0;			
	
			bytes = sprintf(answers + pos,  "(%d,%d){\\cup}(%d,{\\infty})", -A, -B, -C); pos += bytes;
		}
		else if (randomizer% 4 == 1)
		{
			bytes = 0;
			pos = 0;

			bytes = sprintf(content + pos, "<p>Solve the polynomial inequality.</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>\\((x%c%d)(x%c%d)(x%c%d)\\lt 0\\)</p>",
				(A>0?'+':'-'), (int)fabs(A), (B>0?'+':'-'), (int)fabs(B), (C>0?'+':'-'), (int)fabs(C)); pos += bytes;
			bytes = sprintf(content + pos, "<p>The solution set is \\(=\\ \\ ?\\)</p>"); pos += bytes;
	
			bytes = 0;
			pos = 0;			
	
			bytes = sprintf(answers + pos,  "(-{\\infty},%d){\\cup}(%d,%d)", -A, -B, -C); pos += bytes;
		}
		else if (randomizer% 4 == 2)
		{
			bytes = 0;
			pos = 0;

			bytes = sprintf(content + pos, "<p>Solve the polynomial inequality.</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>\\((x%c%d)(x%c%d)(x%c%d)\\ge 0\\)</p>",
				(A>0?'+':'-'), (int)fabs(A), (B>0?'+':'-'), (int)fabs(B), (C>0?'+':'-'), (int)fabs(C)); pos += bytes;
			bytes = sprintf(content + pos, "<p>The solution set is \\(=\\ \\ ?\\)</p>"); pos += bytes;
	
			bytes = 0;
			pos = 0;			
	
			bytes = sprintf(answers + pos,  "[%d,%d]{\\cup}[%d,{\\infty})", -A, -B, -C); pos += bytes;
		}
		else if (randomizer% 4 == 3)
		{
			bytes = 0;
			pos = 0;

			bytes = sprintf(content + pos, "<p>Solve the polynomial inequality.</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>\\((x%c%d)(x%c%d)(x%c%d)\\le 0\\)</p>",
				(A>0?'+':'-'), (int)fabs(A), (B>0?'+':'-'), (int)fabs(B), (C>0?'+':'-'), (int)fabs(C)); pos += bytes;
			bytes = sprintf(content + pos, "<p>The solution set is \\(=\\ \\ ?\\)</p>"); pos += bytes;
	
			bytes = 0;
			pos = 0;			
	
			bytes = sprintf(answers + pos,  "(-{\\infty},%d]{\\cup}[%d,%d]", -A, -B, -C); pos += bytes;
		}

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_3_29(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 19 - 9;
			B = rand() % 19 - 9;
		}
		while (A == 0 || B == 0 || A == B);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		if (randomizer% 4 == 0)
		{
			bytes = 0;
			pos = 0;

			bytes = sprintf(content + pos, "<p>Solve the polynomial inequality.</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>\\( {\\dfrac{ x%c%d }{ x%c%d }} \\gt 0 \\)</p>",
				(A>0?'+':'-'), (int)fabs(A), (B>0?'+':'-'), (int)fabs(B)); pos += bytes;
			bytes = sprintf(content + pos, "<p>The solution set is \\(=\\ \\ ?\\)</p>"); pos += bytes;
	
			bytes = 0;
			pos = 0;			
	
			if (A > B)
			{
				bytes = sprintf(answers + pos,  "(-{\\infty},%d){\\cup}(%d,{\\infty})", -A, -B); pos += bytes;
			}
			else
			{
				bytes = sprintf(answers + pos,  "(-{\\infty},%d){\\cup}(%d,{\\infty})", -B, -A); pos += bytes;
			}
		}
		else if (randomizer% 4 == 1)
		{
			bytes = 0;
			pos = 0;

			bytes = sprintf(content + pos, "<p>Solve the polynomial inequality.</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>\\( {\\dfrac{ x%c%d }{ x%c%d }} \\lt 0 \\)</p>",
				(A>0?'+':'-'), (int)fabs(A), (B>0?'+':'-'), (int)fabs(B)); pos += bytes;
			bytes = sprintf(content + pos, "<p>The solution set is \\(=\\ \\ ?\\)</p>"); pos += bytes;
	
			bytes = 0;
			pos = 0;			
	
			if (A > B)
			{
				bytes = sprintf(answers + pos,  "(%d,%d)", -A, -B); pos += bytes;
			}
			else
			{
				bytes = sprintf(answers + pos,  "(%d,%d)", -B, -A); pos += bytes;
			}
		}
		else if (randomizer% 4 == 2)
		{
			bytes = 0;
			pos = 0;

			bytes = sprintf(content + pos, "<p>Solve the polynomial inequality.</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>\\( {\\dfrac{ x%c%d}{ x%c%d}} \\ge 0 \\)</p>",
				(A>0?'+':'-'), (int)fabs(A), (B>0?'+':'-'), (int)fabs(B)); pos += bytes;
			bytes = sprintf(content + pos, "<p>The solution set is \\(=\\ \\ ?\\)</p>"); pos += bytes;
	
			bytes = 0;
			pos = 0;			
	
			if (A > B)
			{
				bytes = sprintf(answers + pos,  "(-{\\infty},%d]{\\cup}(%d,{\\infty})", -A, -B); pos += bytes;
			}
			else
			{
				bytes = sprintf(answers + pos,  "(-{\\infty},%d){\\cup}[%d,{\\infty})", -B, -A); pos += bytes;
			}
		}
		else if (randomizer% 4 == 3)
		{
			bytes = 0;
			pos = 0;

			bytes = sprintf(content + pos, "<p>Solve the polynomial inequality.</p>"); pos += bytes;
			bytes = sprintf(content + pos, "<p>\\( {\\dfrac{ x%c%d }{ x%c%d }} \\le 0 \\)</p>",
				(A>0?'+':'-'), (int)fabs(A), (B>0?'+':'-'), (int)fabs(B)); pos += bytes;
			bytes = sprintf(content + pos, "<p>The solution set is \\(=\\ \\ ?\\)</p>"); pos += bytes;
	
			bytes = 0;
			pos = 0;			
	
			if (A > B)
			{
				bytes = sprintf(answers + pos,  "[%d,%d)", -A, -B); pos += bytes;
			}
			else
			{
				bytes = sprintf(answers + pos,  "(%d,%d]", -B, -A); pos += bytes;
			}
		}

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_3_30(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 9 + 1;
			B = -(rand() % 9 + 1);
			C = -(rand() % 9 + 1);
		}	
		while (isprime((int)fabs(A)) == false || isprime((int)fabs(B)) == false || A == B || A == -B || fabs(A*B+C) <= 1.0f || (float)(-B/A) >= -C);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Find the domain of the function.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\({\\sqrt{%dx^{2}%c%dx%c%d}}\\)</p>", 
			A, (A*C+B>0?'+':'-'), (int)fabs(A*C+B), (B*C>0?'+':'-'), (int)fabs(B*C)); pos += bytes;
		bytes = sprintf(content + pos, "<p>The domain is \\( =\\ \\ ?\\)</p>"); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "(-{\\infty},{\\dfrac{`%d`}{`%d`}}]{\\cup}[%d,{\\infty})", -B, A, -C); pos += bytes;
	
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_4_01(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 8 + 2;
			B = rand() % 18 + 2;
		}	
		while (A == B || (int)((float)pow(A,sqrt(B)) * 10000.0f) % 10 >= 5 || (int)((float)pow(A,sqrt(B)) * 10000.0f) % 100 < 10 || 
			fabs((float)pow(A,sqrt(B))) <= 1.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Approximate the number using a calculator.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\( %d^{\\sqrt{%d}} \\)</p>", 
			A, B); pos += bytes;
		bytes = sprintf(content + pos, "<p>Round to 3 decimal places.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>The value is \\( =\\ \\ ?\\)</p>"); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%0.3f", (float)pow(A,sqrt(B))); pos += bytes;
	
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;
	
	return 1;
};

int question_4_02(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 99 + 1;
		}	
		while ((int)((float)exp((float)A * -0.01f) * 10000.0f) % 10 >= 5 || (int)((float)exp((float)A * -0.01f) * 10000.0f) % 100 < 10);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Approximate the number using a calculator.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\( e^{%0.2f} \\)</p>", 
			(float)A * -0.01f); pos += bytes;
		bytes = sprintf(content + pos, "<p>Round to 3 decimal places.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>The value is \\( =\\ \\ ?\\)</p>"); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%0.3f\t", (float)exp((float)A * -0.01f)); pos += bytes;
		bytes = sprintf(answers + pos,  ".%d", (int)((float)exp((float)A * -0.01f)*1000.0f)); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;	

	return 1;
};

int question_4_03(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 8 + 2;
		}
		while (A == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Pick the graph of the exponential function.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\( f(x) = %d^{x} \\)</p>",
			A); pos += bytes;

		if (randomizer% 4 == 0)
		{
			bytes = sprintf(content + pos, "<p style='border:1px solid black'>a)"); pos += bytes;
			bytes = sprintf(content + pos, "@f(x) = %d**x ; set object 1 circle at %d,%d size 0.333 fillcolor rgb 'dark-violet' fill solid noborder;", A, 1, A); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos, "</p>"); pos += bytes;

			bytes = sprintf(content + pos, "<p style='border:1px solid black'>b)"); pos += bytes;
			bytes = sprintf(content + pos, "@f(x) = %d**x ; set object 1 circle at %d,%d size 0.333 fillcolor rgb 'dark-violet' fill solid noborder;", A-1, 1, A-1); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos, "</p>"); pos += bytes;

			bytes = sprintf(content + pos, "<p style='border:1px solid black'>c)"); pos += bytes;
			bytes = sprintf(content + pos, "@f(x) = %d**x-1 ; set object 1 circle at %d,%d size 0.333 fillcolor rgb 'dark-violet' fill solid noborder;", A, 1, A-1); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos, "</p>"); pos += bytes;

			bytes = sprintf(content + pos, "<p style='border:1px solid black'>d)"); pos += bytes;
			bytes = sprintf(content + pos, "@f(x) = %d**x+1 ; set object 1 circle at %d,%d size 0.333 fillcolor rgb 'dark-violet' fill solid noborder;", A-1, 1, A); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos, "</p>"); pos += bytes;

			bytes = 0;
			pos = 0;			
		
			bytes = sprintf(answers + pos,  "{a}"); pos += bytes;
		}
		else if (randomizer% 4 == 1)
		{
			bytes = sprintf(content + pos, "<p style='border:1px solid black'>a)"); pos += bytes;
			bytes = sprintf(content + pos, "@f(x) = %d**x ; set object 1 circle at %d,%d size 0.333 fillcolor rgb 'dark-violet' fill solid noborder;", A-1, 1, A-1); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos, "</p>"); pos += bytes;

			bytes = sprintf(content + pos, "<p style='border:1px solid black'>b)"); pos += bytes;
			bytes = sprintf(content + pos, "@f(x) = %d**x ; set object 1 circle at %d,%d size 0.333 fillcolor rgb 'dark-violet' fill solid noborder;", A, 1, A); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos, "</p>"); pos += bytes;

			bytes = sprintf(content + pos, "<p style='border:1px solid black'>c)"); pos += bytes;
			bytes = sprintf(content + pos, "@f(x) = %d**x+1 ; set object 1 circle at %d,%d size 0.333 fillcolor rgb 'dark-violet' fill solid noborder;", A-1, 1, A); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos, "</p>"); pos += bytes;

			bytes = sprintf(content + pos, "<p style='border:1px solid black'>d)"); pos += bytes;
			bytes = sprintf(content + pos, "@f(x) = %d**x-1 ; set object 1 circle at %d,%d size 0.333 fillcolor rgb 'dark-violet' fill solid noborder;", A, 1, A-1); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos, "</p>"); pos += bytes;

			bytes = 0;
			pos = 0;			
		
			bytes = sprintf(answers + pos,  "{b}"); pos += bytes;
		}
		else if (randomizer% 4 == 2)
		{
			bytes = sprintf(content + pos, "<p style='border:1px solid black'>a)"); pos += bytes;
			bytes = sprintf(content + pos, "@f(x) = %d**x-1 ; set object 1 circle at %d,%d size 0.333 fillcolor rgb 'dark-violet' fill solid noborder;", A, 1, A-1); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos, "</p>"); pos += bytes;

			bytes = sprintf(content + pos, "<p style='border:1px solid black'>b)"); pos += bytes;
			bytes = sprintf(content + pos, "@f(x) = %d**x+1 ; set object 1 circle at %d,%d size 0.333 fillcolor rgb 'dark-violet' fill solid noborder;", A-1, 1, A); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos, "</p>"); pos += bytes;

			bytes = sprintf(content + pos, "<p style='border:1px solid black'>c)"); pos += bytes;
			bytes = sprintf(content + pos, "@f(x) = %d**x ; set object 1 circle at %d,%d size 0.333 fillcolor rgb 'dark-violet' fill solid noborder;", A, 1, A); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos, "</p>"); pos += bytes;

			bytes = sprintf(content + pos, "<p style='border:1px solid black'>d)"); pos += bytes;
			bytes = sprintf(content + pos, "@f(x) = %d**x ; set object 1 circle at %d,%d size 0.333 fillcolor rgb 'dark-violet' fill solid noborder;", A-1, 1, A-1); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos, "</p>"); pos += bytes;

			bytes = 0;
			pos = 0;			
		
			bytes = sprintf(answers + pos,  "{c}"); pos += bytes;
		}
		else if (randomizer% 4 == 3)
		{
			bytes = sprintf(content + pos, "<p style='border:1px solid black'>a)"); pos += bytes;
			bytes = sprintf(content + pos, "@f(x) = %d**x ; set object 1 circle at %d,%d size 0.333 fillcolor rgb 'dark-violet' fill solid noborder;", A-1, 1, A-1); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos, "</p>"); pos += bytes;

			bytes = sprintf(content + pos, "<p style='border:1px solid black'>b)"); pos += bytes;
			bytes = sprintf(content + pos, "@f(x) = %d**x-1 ; set object 1 circle at %d,%d size 0.333 fillcolor rgb 'dark-violet' fill solid noborder;", A, 1, A-1); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos, "</p>"); pos += bytes;

			bytes = sprintf(content + pos, "<p style='border:1px solid black'>c)"); pos += bytes;
			bytes = sprintf(content + pos, "@f(x) = %d**x+1 ; set object 1 circle at %d,%d size 0.333 fillcolor rgb 'dark-violet' fill solid noborder;", A-1, 1, A); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos, "</p>"); pos += bytes;

			bytes = sprintf(content + pos, "<p style='border:1px solid black'>d)"); pos += bytes;
			bytes = sprintf(content + pos, "@f(x) = %d**x ; set object 1 circle at %d,%d size 0.333 fillcolor rgb 'dark-violet' fill solid noborder;", A, 1, A); pos += bytes;
			bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
			bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
			bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
			bytes = sprintf(content + pos, "</p>"); pos += bytes;
			
			bytes = 0;
			pos = 0;			
		
			bytes = sprintf(answers + pos,  "{d}"); pos += bytes;
		}
		
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;	
	
	return 1;
};

int question_4_04(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 8 + 2;
			B = rand() % 3 - 1;
			C = rand() % 3 - 1;
		}
		while (B == 0 || C == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Pick the graph of the exponential function.</p>"); pos += bytes;

		if (B > 0 && C > 0) { bytes = sprintf(content + pos, "<p>\\( f(x) = %d^{x} \\)</p>", A); pos += bytes; }
		else if (B < 0 && C > 0) { bytes = sprintf(content + pos, "<p>\\( f(x) = -%d^{x} \\)</p>", A); pos += bytes; }
		else if (B > 0 && C < 0) { bytes = sprintf(content + pos, "<p>\\( f(x) = %d^{-x} \\)</p>", A); pos += bytes; }
		else if (B < 0 && C < 0) { bytes = sprintf(content + pos, "<p>\\( f(x) = -%d^{-x} \\)</p>", A); pos += bytes; }

		bytes = sprintf(content + pos, "<p style='border:1px solid black'>a)"); pos += bytes;
		bytes = sprintf(content + pos, "@f(x) = %d**x ; set object 1 circle at %d,%d size 0.333 fillcolor rgb 'dark-violet' fill solid noborder;", A, 1, A); pos += bytes;
		bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
		bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
		bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
		bytes = sprintf(content + pos, "</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p style='border:1px solid black'>b)"); pos += bytes;
		bytes = sprintf(content + pos, "@f(x) = -%d**x ; set object 1 circle at %d,%d size 0.333 fillcolor rgb 'dark-violet' fill solid noborder;", A, 1, -A); pos += bytes;
		bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
		bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
		bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
		bytes = sprintf(content + pos, "</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p style='border:1px solid black'>c)"); pos += bytes;
		bytes = sprintf(content + pos, "@f(x) = %d**(-x) ; set object 1 circle at %d,%d size 0.333 fillcolor rgb 'dark-violet' fill solid noborder;", A, -1, A); pos += bytes;
		bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
		bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
		bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
		bytes = sprintf(content + pos, "</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p style='border:1px solid black'>d)"); pos += bytes;
		bytes = sprintf(content + pos, "@f(x) = -%d**(-x) ; set object 1 circle at %d,%d size 0.333 fillcolor rgb 'dark-violet' fill solid noborder;", A, -1, -A); pos += bytes;
		bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
		bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
		bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
		bytes = sprintf(content + pos, "</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			
		
		if (B > 0 && C > 0) { bytes = sprintf(answers + pos,  "{a}"); pos += bytes; }
		else if (B < 0 && C > 0) { bytes = sprintf(answers + pos,  "{b}"); pos += bytes; }
		else if (B > 0 && C < 0) { bytes = sprintf(answers + pos,  "{c}"); pos += bytes; }
		else if (B < 0 && C < 0) { bytes = sprintf(answers + pos,  "{d}"); pos += bytes; }		
		
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;	
	
	return 1;
};

int question_4_05(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	unsigned long Z = 0;

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 49 + 1;
			B = rand() % 8 + 2;
			C = rand() % 99 + 1;
			D = rand() % 4;

			if (D == 0) E = 2; 
			else if (D == 1) E = 4;
			else if (D == 2) E = 12;
			else if (D == 3) E = 365; // changes

			if (D < 3) Z = (unsigned long)((double)((double)A * 1000.0f * (double)pow(1 + (double)C / 1000.0f / (double)E, (double)E * (double)B)) * 1000.0f);
			else Z = (unsigned long)((double)((double)A * 1000.0f * (double)exp((double)C / 1000.0f * (double)B)) * 1000.0f);
		}	
		while (A == 0 || Z % 10 >= 5 || Z % 100 < 10 || fabs((double)Z / 1000.0f) <= 1.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Use the component interest formulas \\( A = P \\left( 1 + \\dfrac{ r }{ n } \\right) ^ {nt} \\) and "); pos += bytes;
		bytes = sprintf(content + pos, "\\( A = Pe ^ {rt} \\) to solve the problem given.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>Find the accumulated value of an investment of $%d,000 for %d years at an interest rate of %.1f%% if the money is compounded ", 
			A, B, (float)C / 10.0f); pos += bytes;

		if (D == 0)
		{
			bytes = sprintf(content + pos, "semiannually.</p>"); pos += bytes;
		}
		else if (D == 1)
		{
			bytes = sprintf(content + pos, "quarterly.</p>"); pos += bytes;
		}
		else if (D == 2)
		{
			bytes = sprintf(content + pos, "monthly.</p>"); pos += bytes;
		}
		else if (D == 3)
		{
			bytes = sprintf(content + pos, "continuously.</p>"); pos += bytes;
		}

		bytes = sprintf(content + pos, "<p>Round to the nearest cent.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>The accumulated value is \\( =\\ \\ ?\\)</p>"); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%.2f", (double)Z / 1000.0f); pos += bytes;
	
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;
	
	return 1;
};

int question_4_06(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 4 + 6;
			B = rand() % 9 + 1;
			C = rand() % 53;

			D = (int)(((float)A * 10.0f * exp(-(float)B / 10.0f * (float)C) + (100.0f - (float)A * 10.0f)) * 100.0f);
		}	
		while (A == 0 || D % 10 >= 5 || D % 100 < 10 || fabs((float)D / 100.0f) <= 1.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>The function \\( f(x) = %de^{-%0.1fx}+%d \\) describes the percentage, \\( f(x) \\), that a person remembers \\( x \\) weeks after learning.</p>", 
			A * 10, (float)B / 10.0f, 100 - A * 10); pos += bytes;

		bytes = sprintf(content + pos, "<p>Find the percentage of information that is remembered after %d weeks.</p>", C); pos += bytes;
		bytes = sprintf(content + pos, "<p>Round to 1 decimal place.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>The accumulated value is \\( =\\ \\ ?\\)</p>"); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%.1f", (float)D / 100.0f); pos += bytes;
	
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;
	
	return 1;
};

int question_4_07(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 8 + 2;
			B = rand() % 4 + 2;
		}	
		while (A == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Write the following equation in it's equivalent exponential form.</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p>\\( \\log_{%d}(%d)=x \\)</p>", A, (int)pow(A,B)); pos += bytes;
		bytes = sprintf(content + pos, "<p>The exponential form is \\( =\\ \\ ?\\)</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>Do not solve for x.</p>"); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%d{^{`x`}}=%d\t", A, (int)pow(A,B)); pos += bytes;
		bytes = sprintf(answers + pos,  "%d=%d{^{`x`}}", (int)pow(A,B), A); pos += bytes;
	
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;	

	return 1;
};

int question_4_08(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 8 + 2;
			B = rand() % 4 + 2;
		}	
		while (A == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Write the following equation in it's equivalent logarithmic form.</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p>\\( %d^{x}=%d \\)</p>", A, (int)pow(A,B)); pos += bytes;
		bytes = sprintf(content + pos, "<p>The logarithmic form is \\( =\\ \\ ?\\)</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>Do not solve for x.</p>"); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "{\\log_{`%d`}({`%d`})}=x\t", A, (int)pow(A,B)); pos += bytes;
		bytes = sprintf(answers + pos,  "x={\\log_{`%d`}({`%d`})}", A, (int)pow(A,B)); pos += bytes;
	
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;	

	return 1;
};

int question_4_09(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 8 + 2;
			B = rand() % 4 + 2;
		}	
		while (A == 0 || pow(A,B) > 1000);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Evaluate the logarithmic expression.</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p>\\( \\log_{%d}(\\dfrac{ 1 }{ %d })=\\ \\ ? \\)</p>", A, (int)pow(A,B)); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "-%d", B); pos += bytes;
	
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;	
	
	return 1;
};

int question_4_10(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 8 + 2;
			B = rand() % 3 - 1;
			C = rand() % 3 - 1;
		}
		while (B == 0 || C == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Pick the graph of the logarithmic function.</p>"); pos += bytes;

		if (B > 0 && C > 0) { bytes = sprintf(content + pos, "<p>\\( f(x) = \\log_{%d}(x) \\)</p>", A); pos += bytes; }
		else if (B < 0 && C > 0) { bytes = sprintf(content + pos, "<p>\\( f(x) = -\\log_{%d}(x) \\)</p>", A); pos += bytes; }
		else if (B > 0 && C < 0) { bytes = sprintf(content + pos, "<p>\\( f(x) = \\log_{%d}(-x) \\)</p>", A); pos += bytes; }
		else if (B < 0 && C < 0) { bytes = sprintf(content + pos, "<p>\\( f(x) = -\\log_{%d}(-x) \\)</p>", A); pos += bytes; }

		bytes = sprintf(content + pos, "<p style='border:1px solid black'>a)"); pos += bytes;
		bytes = sprintf(content + pos, "@f(x) = log(x)/log(%d) ; set object 1 circle at %d,%d size 0.333 fillcolor rgb 'dark-violet' fill solid noborder;", A, A, 1); pos += bytes;
		bytes = sprintf(content + pos, "set arrow from %f,%f to %f,%f nohead lw 1 lc rgb 'dark-violet';", 0.1f,log(0.1f)/log(A), 0.1f,-10.0f); pos += bytes;
		bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
		bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
		bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
		bytes = sprintf(content + pos, "</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p style='border:1px solid black'>b)"); pos += bytes;
		bytes = sprintf(content + pos, "@f(x) = -log(x)/log(%d) ; set object 1 circle at %d,%d size 0.333 fillcolor rgb 'dark-violet' fill solid noborder;", A, A, -1); pos += bytes;
		bytes = sprintf(content + pos, "set arrow from %f,%f to %f,%f nohead lw 1 lc rgb 'dark-violet';", 0.1f,-log(0.1f)/log(A), 0.1f,10.0f); pos += bytes;
		bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
		bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
		bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
		bytes = sprintf(content + pos, "</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p style='border:1px solid black'>c)"); pos += bytes;
		bytes = sprintf(content + pos, "@f(x) = log(-x)/log(%d) ; set object 1 circle at %d,%d size 0.333 fillcolor rgb 'dark-violet' fill solid noborder;", A, -A, 1); pos += bytes;
		bytes = sprintf(content + pos, "set arrow from %f,%f to %f,%f nohead lw 1 lc rgb 'dark-violet';", -0.1f,log(0.1f)/log(A), -0.1f,-10.0f); pos += bytes;
		bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
		bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
		bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
		bytes = sprintf(content + pos, "</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p style='border:1px solid black'>d)"); pos += bytes;
		bytes = sprintf(content + pos, "@f(x) = -log(-x)/log(%d) ; set object 1 circle at %d,%d size 0.333 fillcolor rgb 'dark-violet' fill solid noborder;", A, -A, -1); pos += bytes;
		bytes = sprintf(content + pos, "set arrow from %f,%f to %f,%f nohead lw 1 lc rgb 'dark-violet';", -0.1f,-log(0.1f)/log(A), -0.1f,10.0f); pos += bytes;
		bytes = sprintf(content + pos, "set xrange[-10:10] ; set yrange[-10:10] ; set xtics axis 1 scale 0.5 ; set ytics axis 1 scale 0.5 ;"); pos += bytes;
		bytes = sprintf(content + pos, "set format x '' ; set format y '' ; set grid xtics ytics linetype 1 linecolor rgb '#888888' ; "); pos += bytes;
		bytes = sprintf(content + pos, "set xzeroaxis linetype 1 linecolor rgb '#000000' ; set yzeroaxis linetype 1 linecolor rgb '#000000' ; set key noautotitle@"); pos += bytes;
		bytes = sprintf(content + pos, "</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			
		
		if (B > 0 && C > 0) { bytes = sprintf(answers + pos,  "{a}"); pos += bytes; }
		else if (B < 0 && C > 0) { bytes = sprintf(answers + pos,  "{b}"); pos += bytes; }
		else if (B > 0 && C < 0) { bytes = sprintf(answers + pos,  "{c}"); pos += bytes; }
		else if (B < 0 && C < 0) { bytes = sprintf(answers + pos,  "{d}"); pos += bytes; }		
		
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;		

	return 1;
};

int question_4_11(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 19 - 9;
			B = rand() % 8 + 2;
		}
		while (A == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Find the domain of the function.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\(f(x) = \\log_{%d}(x%c%d)\\)</p>",
			B, (A>0?'+':'-'), (int)fabs(A)); pos += bytes;
		bytes = sprintf(content + pos, "<p>The domain of the function in interval notation is \\(=\\ \\ ?\\)</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "(%d,{\\infty})", -A); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;	

	return 1;
};

int question_4_12(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 8 + 2;
		}	
		while (A == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Evaluate the logarithmic expression.</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p>\\( \\ln(e^{%d}) =\\ \\ ? \\)</p>", A); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%d", A); pos += bytes;
	
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;		

	return 1;
};

int question_4_13(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 18 + 2;
		}	
		while (A == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Use properties of logarithms to expand the logarithmic expression as much as possible. Evaluate logarithms whenever possible.</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p>\\( \\log_{%d}(%dx) =\\ \\ ? \\)</p>", A, A); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "1+{\\log_{`%d`}({`x`})}\t", A); pos += bytes;
		bytes = sprintf(answers + pos,  "{\\log_{`%d`}({`x`})}+1", A); pos += bytes;
	
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;		
	
	return 1;
};

int question_4_14(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 18 + 2;
			B = rand() % 18 + 2;
		}	
		while (A == 0 || A == B);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Use properties of logarithms to expand the logarithmic expression as much as possible. Evaluate logarithms whenever possible.</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p>\\( \\ln \\left( \\dfrac{ e^{%d} }{ %d } \\right) =\\ \\ ? \\)</p>", A, B); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%d-{\\ln({`%d`})}\t", A, B); pos += bytes;
		bytes = sprintf(answers + pos,  "-{\\ln({`%d`})}+%d", B, A); pos += bytes;
	
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;			

	return 1;
};

int question_4_15(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 8 + 2;
			B = rand() % 8 + 2;
		}	
		while (A == 0 || A == B);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Use properties of logarithms to expand the logarithmic expression as much as possible. Evaluate logarithms whenever possible.</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p>\\( \\ln \\left( \\dfrac{ x^{%d}y }{ z^{%d} } \\right) =\\ \\ ? \\)</p>", A, B); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%d{\\ln({`x`})}+{\\ln({`y`})}-%d{\\ln({`z`})}\t", A, B); pos += bytes;
		bytes = sprintf(answers + pos,  "%d{\\ln({`x`})}-%d{\\ln({`z`})}+{\\ln({`y`})}\t", A, B); pos += bytes;
		bytes = sprintf(answers + pos,  "{\\ln({`y`})}+%d{\\ln({`x`})}-%d{\\ln({`z`})}\t", A, B); pos += bytes;
		bytes = sprintf(answers + pos,  "{\\ln({`y`})}-%d{\\ln({`z`})}+%d{\\ln({`x`})}\t", B, A); pos += bytes;
		bytes = sprintf(answers + pos,  "-%d{\\ln({`z`})}+{\\ln({`y`})}+%d{\\ln({`x`})}\t", B, A); pos += bytes;
		bytes = sprintf(answers + pos,  "-%d{\\ln({`z`})}+%d{\\ln({`x`})}+{\\ln({`y`})}", B, A); pos += bytes;
		
		
	
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;			

	return 1;
};

int question_4_16(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 18 + 2;
			B = rand() % 19 - 9;
			C = rand() % 18 + 2;
		}	
		while (A == C || !isprime(A) || !isprime(C) || B == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Use properties of logarithms to condense the logarithmic expression. Evaluate logarithms whenever possible.</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p>\\( %d \\ln(x%c%d) - %d \\ln(x) =\\ \\ ? \\)</p>", A, (B>0?'+':'-'), (int)fabs(B), C); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "{\\ln({{\\dfrac{`(x%c%d)`{^{`%d`}}}{`x`{^{`%d`}}}}})}", (B>0?'+':'-'), (int)fabs(B), A, C); pos += bytes;
		
		
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}		
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;			
	
	return 1;
};

int question_4_17(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 18 + 2;
			B = rand() % 18 + 2;
			C = rand() % 18 + 2;
		}	
		while (A == B || A == C || B == C || !isprime(A) || !isprime(B) || !isprime(C));

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Use properties of logarithms to condense the logarithmic expression. Evaluate logarithms whenever possible.</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p>\\( %d \\ln(x) + %d \\ln(y) - %d \\ln(z) =\\ \\ ? \\)</p>", A, B, C); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "{\\ln({{\\dfrac{`x`{^{`%d`}}`y`{^{`%d`}}}{`z`{^{`%d`}}}}})}\t", A, B, C); pos += bytes;
		bytes = sprintf(answers + pos,  "{\\ln({{\\dfrac{`y`{^{`%d`}}`x`{^{`%d`}}}{`z`{^{`%d`}}}}})}", B, A, C); pos += bytes;
		
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}		
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;			

	return 1;
};

int question_4_18(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 18 + 2;
			B = rand() % 18 + 2;
		}	
		while (A == B || !isprime(A) || !isprime(B) || (int)((float)log(B) / (float)log(A) * 10000.0f) % 10 >= 5 ||
			(int)((float)log(B) / (float)log(A) * 1000.0f) % 10 < 1 || fabs((float)log(B) / (float)log(A)) <= 1.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Use the change of base formula and a calculator to evaluate the following.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\( \\log_{%d}(%d) =\\ \\ ? \\)</p>", A, B); pos += bytes;
		bytes = sprintf(content + pos, "<p>Round to 3 decimal places.</p>"); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%.3f", (float)log(B) / (float)log(A)); pos += bytes;
		
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}		
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;				

	return 1;
};

int question_4_19(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 5 + 2;
			B = rand() % 5 + 2;
			C = rand() % 18 + 2;
		}	
		while (A == B);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve the following exponential equation.</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p>\\( %d^{%d-x} = \\dfrac{ 1 }{ %d } \\)</p><p>\\(x =\\ \\ ?\\)</p>", A, C, (int)pow(A,B)); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%d", B+C); pos += bytes;
		
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}		
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;			

	return 1;
};

int question_4_20(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 19 - 9;
		}	
		while ((int)fabs(A) <= 1);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve the following exponential equation.</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p>\\( e^{x%c%d} = \\dfrac{ 1 }{ e } \\)</p><p>\\(x =\\ \\ ?\\)</p>", (A>0?'+':'-'), (int)fabs(A)); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%d", -1-A); pos += bytes;
		
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}		
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;	

	return 1;
};

int question_4_21(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 5 + 2;
			B = rand() % 5 + 2;
			C = rand() % 900 + 100;
		}	
		while (!isprime(C));

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve the following exponential equation in terms of natural logarithms.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\( %d e^{%d x} = %d \\)</p><p>\\(x =\\ \\ ?\\)</p>", A, B, A*C); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "{\\dfrac{{\\ln({`%d`})}}{`%d`}}", C, B); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}		
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;	

	
	return 1;
};


int question_4_22(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 5 + 2;
			B = rand() % 5 + 2;
			C = rand() % 19 - 9;
		}	
		while (A == B || C == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve the following logarithmic equation.</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p>\\( \\log_{%d}(x%c%d) = %d \\)</p><p>\\(x =\\ \\ ?\\)</p>", A, (C>0?'+':'-'), (int)fabs(C), B); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%d", (int)pow(A,B)-C); pos += bytes;
		
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}		
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;		

	return 1;
};


int question_4_23(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 38 + 2;
			B = rand() % 5 + 2;
			C = rand() % 5 + 2;
		}	
		while (A == B || C == 0 || A-(int)pow(B,C) <= 0 || !isprime(A) || (int)pow(B,C) % A == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve the following logarithmic equation.</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p>\\( \\log_{%d}(x) + \\log_{%d}(%dx+%d) = %d \\)</p><p>\\(x =\\ \\ ?\\)</p>", 
			B, B, A, A-(int)pow(B,C), C); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "{\\dfrac{`%d`}{`%d`}}", (int)pow(B,C), A); pos += bytes;
		
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}		
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;		

	return 1;
};


int question_4_24(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 8 + 2;
			B = rand() % 8 + 2;
		}	
		while (A == B || A-B <= 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve the following logarithmic equation.</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p>\\( \\ln(x) + \\ln(x+%d) = \\ln(%d) \\)</p><p>\\(x =\\ \\ ?\\)</p>", 
			A-B, A*B); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%d", B); pos += bytes;
		
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}		
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;		
	
	return 1;
};

int question_4_25(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 10000 + 1;
			B = rand() % 19 + 1;
			C = (int)(A/10) + rand() % 1000 + 1;
			D = 2000 + rand() % 10;
		}	
		while (A == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>The exponential formula "); pos += bytes;
		bytes = sprintf(content + pos, "\\( A = %.1f e^{ %.3ft } \\)", (float)A / 10.0f, (float)B / 1000.0f); pos += bytes;
		bytes = sprintf(content + pos, " describes a population, \\( A \\), in millions, \\( t \\) years after %d.</p>", D); pos += bytes;
		bytes = sprintf(content + pos, "<p>Use the model to determine when the population of the country will be %d million.</p>", C); pos += bytes;
		bytes = sprintf(content + pos, "<p>Round to the nearest year, do not use commas.</p>"); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%d", (int)(round(log( (float)C / (float)A * 10.0f ) / log(exp(1)) / (float)B * 1000.0f) + (float)D)); pos += bytes;
		
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}		
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;			
	
	return 1;
};

int question_4_26(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 8 + 12;
			B = rand() % 2000 + 2000;
		}	
		while (A == 0 || (int)((float)A * exp(-0.000121f*(float)B) * 10000.0f) % 10 >= 5 ||
			(int)((float)A * exp(-0.000121f*(float)B) * 1000.0f) % 10 < 1 || fabs((float)A * exp(-0.000121f*(float)B)) <= 1.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>An artifact originally had %d grams of​ carbon-14 present.</p>", A); pos += bytes;
		bytes = sprintf(content + pos, "<p>The decay model \\( A = %de^{-0.000121t} \\) describes the amount of carbon-14 after \\( t \\) years.</p>", A); pos += bytes;
		bytes = sprintf(content + pos, "<p>Use the model to determine how many grams of​ carbon-14 will be present in %d years.</p>", B); pos += bytes;
		bytes = sprintf(content + pos, "<p>Round to 3 decimal places.</p>"); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%.3f", (float)A * exp(-0.000121f*(float)B)); pos += bytes;
		
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}		
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;				

	return 1;
};

int question_4_27(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 20 + 59;
		}	
		while (A == 0 || (int)round(log((float)A / 100.0f) / log(exp(1)) / -0.000121f) >= 4000);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Prehistoric cave paintings were discovered in a cave in France. The paint contained %d%%", A); pos += bytes;
		bytes = sprintf(content + pos, " of the original​ carbon-14.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>Use the carbon-14 decay model \\( A = A_{0}e^{-0.000121t} \\) to estimate the age of the paintings.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>Round to the nearest whole number, do not use commas.</p>"); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%d", (int)round(log((float)A / 100.0f) / log(exp(1)) / -0.000121f)); pos += bytes;
		
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}	
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;			
	
	return 1;
};

int question_4_28(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 9 + 21;
			B = rand() % 50 + 25;
		}	
		while (A == B || (int)(log((float)B/100.0f) / log(0.5) * (float)A * 10000.0f) % 10 >= 5 ||
			(int)(log((float)B/100.0f) / log(0.5) * (float)A * 1000.0f) % 10 < 1 || fabs(log((float)B/100.0f) / log(0.5) * (float)A) <= 1.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Use the exponential decay​ model, \\( A = A_{0}e^{kt} \\), to solve the following.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>The​ half-life of a certain substance is %d years.</p>", A); pos += bytes;
		bytes = sprintf(content + pos, "<p>How long will it take for a sample of this substance to decay to %d%% of its original​ amount?</p>", B); pos += bytes;
		bytes = sprintf(content + pos, "<p>Round to 3 decimal places.</p>"); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%.3f", log((float)B/100.0f) / log(0.5) * (float)A); pos += bytes;
		
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}	
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;					

	return 1;
};

int question_4_29(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 9 + 31;
			B = rand() % 20 + 20;
		}	
		while (A == B || (int)(log((float)B/100.0f) / log(0.5) * (float)A * 10000.0f) % 10 >= 5 ||
			(int)(log((float)B/100.0f) / log(0.5) * (float)A * 1000.0f) % 10 < 1 || fabs(log((float)B/100.0f) / log(0.5) * (float)A) <= 1.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Use the exponential decay​ model, \\( A = A_{0}e^{kt} \\), to solve the following.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>The​ half-life of a certain tranquilizer in the bloodstream is %d hours.</p>", A); pos += bytes;
		bytes = sprintf(content + pos, "<p>How long will it take for the drug to decay to %d%% of its original​ dosage?</p>", B); pos += bytes;
		bytes = sprintf(content + pos, "<p>Round to 3 decimal places.</p>"); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%.3f", log((float)B/100.0f) / log(0.5) * (float)A); pos += bytes;
		
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;				

	return 1;
};

int question_4_30(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 100 + 99;
			B = rand() % 20 + 49;
			C = rand() % 4 + 5;
		}	
		while (A == B);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>The logistic growth function \\( f(t) = \\dfrac{ %d,000 }{ 1+%d00e^{-t} } \\), describes the number of​ people, \\( f(t) \\)", A, B); pos += bytes;
		bytes = sprintf(content + pos, "who have heard the lastest rumor \\( t \\) days after its initial knowledge in a particular community.</p>"); pos += bytes;

		if (randomizer% 3 == 0)
		{
			bytes = sprintf(content + pos, "<p>How many people knew of this rumor when it first began?</p>"); pos += bytes;
		}
		else if (randomizer% 3 == 1)
		{
			bytes = sprintf(content + pos, "<p>How many people knew of this rumor after %d days?</p>", C); pos += bytes;
		}
		else if (randomizer% 3 == 2)
		{
			bytes = sprintf(content + pos, "<p>What is the limiting size of the population knowing the rumor?</p>"); pos += bytes;
		}

		bytes = sprintf(content + pos, "<p>Round to the nearest whole number, do not use commas.</p>"); pos += bytes;
		
		bytes = 0;
		pos = 0;			

		if (randomizer% 3 == 0)
		{
			bytes = sprintf(answers + pos,  "%d\t", (int)round((float)A * 1000.0f / (1.0f + (float)B * 100.0f))); pos += bytes;
			bytes = sprintf(answers + pos,  "%d,%03d", (int)round((float)A * 1000.0f / (1.0f + (float)B * 100.0f)) / 1000,
				(int)round((float)A * 1000.0f / (1.0f + (float)B * 100.0f)) % 1000); pos += bytes;
		}
		else if (randomizer% 3 == 1)
		{
			bytes = sprintf(answers + pos,  "%d\t", (int)round( (float)A * 1000.0f / (1.0f + (float)B * 100.0f * exp(-(float)C)) )); pos += bytes;
			bytes = sprintf(answers + pos,  "%d,%03d", (int)round( (float)A * 1000.0f / (1.0f + (float)B * 100.0f * exp(-(float)C)) ) / 1000,
				(int)round( (float)A * 1000.0f / (1.0f + (float)B * 100.0f * exp(-(float)C)) ) % 1000); pos += bytes;
		}
		else if (randomizer% 3 == 2)
		{
			bytes = sprintf(answers + pos,  "%d\t", A * 1000); pos += bytes;
			bytes = sprintf(answers + pos,  "%d,000", A); pos += bytes;
		}
		
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;			
	
	return 1;
};

int question_5_01(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 18 + 2;
			B = rand() % 18 + 2;
			C = rand() % 18 + 2;
			D = rand() % 18 + 2;
		}
		while (A == 0 || B == 0 || C == 0 || D == 0 ||
			!isinteger((float)(-D-B)/(float)(A-C)) || fabs((float)(-D-B)/(float)(A-C)) > 10.0f || 
			!isinteger((float)A*(float)(-D-B)/(float)(A-C)+(float)B) || fabs((float)A*(float)(-D-B)/(float)(A-C)+(float)B) > 10.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve the system of equations by the substitution method.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\( \\begin{array}{ccccc} x \\!\\! & = \\!\\! & %dy \\!\\! & + \\!\\! & %d \\!\\! \\\\ ", A, B); pos += bytes;
		bytes = sprintf(content + pos, "x \\!\\! & = \\!\\! & %dy \\!\\! & - \\!\\! & %d \\!\\! \\end{array} \\)</p>", C, D); pos += bytes;
		bytes = sprintf(content + pos, "<p>Type an ordered pair.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "(%d,%d)", 
			(int)((float)A*(float)(-D-B)/(float)(A-C)+(float)B),
			(int)((float)(-D-B)/(float)(A-C))); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_5_02(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 18 + 2;
			B = rand() % 18 + 2;
			C = rand() % 18 + 2;
			D = rand() % 18 + 2;
		}
		while (A == 0 || B == 0 || C == 0 || D == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve the system of equations by the substitution method.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\( \\begin{array}{ccccc} %dx \\!\\! & + \\!\\! & %dy \\!\\! & = \\!\\! & %d \\!\\! \\\\ ", A, B, 0); pos += bytes;
		bytes = sprintf(content + pos, "%dx \\!\\! & + \\!\\! & %dy \\!\\! & = \\!\\! & %d \\!\\! \\end{array} \\)</p>", C, D, 0); pos += bytes;
		bytes = sprintf(content + pos, "<p>Type an ordered pair.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "(0,0)"); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_5_03(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 18 + 2;
			B = rand() % 18 + 2;
			C = rand() % 18 + 2;
			D = rand() % 18 + 2;
			E = rand() % 18 + 2;
		}
		while (A == 0 || B == 0 || C == 0 || D == 0 || E == 0 ||
			!isinteger((float)(-E-B*C)/(float)(D-A*C)) || fabs((float)(-E-B*C)/(float)(D-A*C)) > 10.0f || 
			!isinteger((float)B-(float)A*(float)(-E-B*C)/(float)(D-A*C)) || fabs((float)B-(float)A*(float)(-E-B*C)/(float)(D-A*C)) > 10.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve the system of equations by the addition method.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\( \\begin{array}{ccccc} x \\!\\! & + \\!\\! & %dy \\!\\! & = \\!\\! & %d \\!\\! \\\\ ", A, B); pos += bytes;
		bytes = sprintf(content + pos, "%dx \\!\\! & + \\!\\! & %dy \\!\\! & = \\!\\! & -%d \\!\\! \\end{array} \\)</p>", C, D, E); pos += bytes;
		bytes = sprintf(content + pos, "<p>Type an ordered pair.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "(%d,%d)",
			(int)((float)B-(float)A*(float)(-E-B*C)/(float)(D-A*C)),
			(int)((float)(-E-B*C)/(float)(D-A*C))); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_5_04(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}
	
	float tx, ty;

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 18 + 2;
			B = rand() % 18 + 2;
			C = rand() % 18 + 2;
			D = rand() % 18 + 2;
			E = rand() % 18 + 2;
			F = rand() % 18 + 2;

			ty = (float)(F + (float)(C*D)/(float)A) / ((float)(-B*D)/(float)A-(float)E);
			tx = (float)(-C-B*ty)/(float)A;
		}
		while (A == 0 || B == 0 || C == 0 || D == 0 || E == 0 || F == 0 ||
			!isinteger(tx) || fabs(tx) > 10.0f || 
			!isinteger(ty) || fabs(ty) > 10.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve the system of equations by the addition method.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\( \\begin{array}{ccccc} %dx \\!\\! & + \\!\\! & %dy \\!\\! & = \\!\\! & -%d \\!\\! \\\\ ", A, B, C); pos += bytes;
		bytes = sprintf(content + pos, "%dx \\!\\! & - \\!\\! & %dy \\!\\! & = \\!\\! & %d \\!\\! \\end{array} \\)</p>", D, E, F); pos += bytes;
		bytes = sprintf(content + pos, "<p>Type an ordered pair.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "(%d,%d)",
			(int)tx,
			(int)ty); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_5_05(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}
	
	float tx, ty;

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 18 + 2;
			B = rand() % 18 + 2;
			C = rand() % 38 + 2;
			D = rand() % 18 + 2;
			E = rand() % 18 + 2;
			F = rand() % 38 + 2;

			ty = (float)(-F - (float)(C*D)/(float)A) / ((float)(B*D)/(float)A+(float)E);
			tx = (float)(C+B*ty)/(float)A;
		}
		while (A == 0 || B == 0 || C == 0 || D == 0 || E == 0 || F == 0 ||
			A == D || B == E || B == C || E == F ||
			!isinteger(tx) || fabs(tx) > 10.0f || 
			!isinteger(ty) || fabs(ty) > 10.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve the system of equations by the method of your choice.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\( \\begin{array}{ccccc} %dx \\!\\! & - \\!\\! & %dy \\!\\! & = \\!\\! & %d \\!\\! \\\\ ", A, B, C); pos += bytes;
		bytes = sprintf(content + pos, "%dx \\!\\! & + \\!\\! & %dy \\!\\! & = \\!\\! & -%d \\!\\! \\end{array} \\)</p>", D, E, F); pos += bytes;
		bytes = sprintf(content + pos, "<p>Type an ordered pair.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "(%d,%d)",
			(int)tx,
			(int)ty); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_5_06(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = (rand() % 6 + 4) * 10;
			B = rand() % 19 + 20;
			C = rand() % 19 + 40;
			D = rand() % (A-20) + 1;
			E = A - D;
			A *= 100;
			D *= 100;
			E *= 100;
		}
		while (A == 0 || B == 0 || C == 0 || D == 0 || E == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>A %d-seat theater has tickets for sale.  Tickets for children are $%d and tickets for adults are $%d.</p>", A, B, C); pos += bytes;
		bytes = sprintf(content + pos, "<p>How many %s tickets should be sold so that a sellout performance will generate a total revenue of $%d?</p>",
			(randomizer%2==0?"children":"adults"), D*B + E*C); pos += bytes; 		

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos, "%d", (randomizer%2==0?D:E)); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_5_07(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}
	
	float tx1, ty1, tx2, ty2;

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 18 + 2;
			B = rand() % 18 + 2;
			C = rand() % 38 + 2;

			tx1 = (float)((-1.0f+(float)B+sqrt(1.0f-2.0f*(float)B+(float)pow(B,2)-4.0f*(float)C+4.0f*(float)A))/2.0f);
			ty1 = (float)A - tx1;
			tx2 = (float)((-1.0f+(float)B-sqrt(1.0f-2.0f*(float)B+(float)pow(B,2)-4.0f*(float)C+4.0f*(float)A))/2.0f);
			ty2 = (float)A - tx2;
		}
		while (A == 0 || B == 0 || C == 0 || tx1 == tx2 || ty1 == ty2 ||
			!isinteger(tx1) || fabs(tx1) > 10.0f || 
			!isinteger(ty1) || fabs(ty1) > 10.0f ||
			!isinteger(tx2) || fabs(tx2) > 10.0f || 
			!isinteger(ty2) || fabs(ty2) > 10.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve the system of equations by the substitution method.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\( \\begin{array}{ccccccccc} x \\!\\! & + \\!\\! & y \\!\\! & = \\!\\! & %d \\!\\! & \\!\\! & \\!\\! & \\!\\! & \\!\\! \\\\ ", A); pos += bytes;
		bytes = sprintf(content + pos, "\\!\\! & \\!\\! & y \\!\\! & = \\!\\! & x^{2} \\!\\! & - \\!\\! & %dx \\!\\! & + \\!\\! & %d \\!\\! \\end{array} \\)</p>", B, C); pos += bytes;
		bytes = sprintf(content + pos, "<p>Type ordered pairs. Use commas to separate answers.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d)\t", (int)tx1, (int)ty1, (int)tx2, (int)ty2); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d)", (int)tx2, (int)ty2, (int)tx1, (int)ty1); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_5_08(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}
	
	float tx1, ty1, tx2, ty2;

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 18 + 2;
			B = rand() % 38 + 2;
			C = rand() % 18 + 2;
			D = rand() % 38 + 2;

			tx1 = (float)((float)A+(float)C+sqrt(pow(A,2)+2.0f*(float)A*(float)C+pow(C,2)-8.0f*(float)B-8.0f*(float)D))/4.0f;
			ty1 = pow(tx1,2)-(float)A*tx1+(float)B;
			tx2 = (float)((float)A+(float)C-sqrt(pow(A,2)+2.0f*(float)A*(float)C+pow(C,2)-8.0f*(float)B-8.0f*(float)D))/4.0f;
			ty2 = pow(tx2,2)-(float)A*tx2+(float)B;
		}
		while (A == 0 || B == 0 || C == 0 || D == 0 ||
			tx1 == tx2 || ty1 == ty2 ||
			!isinteger(tx1) || fabs(tx1) > 10.0f || 
			!isinteger(ty1) || fabs(ty1) > 10.0f ||
			!isinteger(tx2) || fabs(tx2) > 10.0f || 
			!isinteger(ty2) || fabs(ty2) > 10.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve the system of equations by the substitution method.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\( \\begin{array}{ccccccc} y \\!\\! & = \\!\\! & x^2 \\!\\! & - \\!\\! & %dx \\!\\! & + \\!\\! & %d \\!\\! \\\\ ", A, B); pos += bytes;
		bytes = sprintf(content + pos, "y \\!\\! & = \\!\\! & -x^2 \\!\\! & + \\!\\! & %dx \\!\\! & - \\!\\! & %d \\!\\! \\end{array} \\)</p>", C, D); pos += bytes;
		bytes = sprintf(content + pos, "<p>Type ordered pairs. Use commas to separate answers.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d)\t", (int)tx1, (int)ty1, (int)tx2, (int)ty2); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d)", (int)tx2, (int)ty2, (int)tx1, (int)ty1); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_5_09(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}
	
	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 9 + 1;
			B = rand() % 9 + 1;
		}
		while (A == B);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve the system of equations by the addition method.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\( \\begin{array}{ccccc} x^2 \\!\\! & + \\!\\! & y^2 \\!\\! & = \\!\\! & %d \\!\\! \\\\ ", (int)(pow(A,2)+pow(B,2))); pos += bytes;
		bytes = sprintf(content + pos, "x^2 \\!\\! & - \\!\\! & y^2 \\!\\! & = \\!\\! & %d \\!\\! \\end{array} \\)</p>", (int)(pow(A,2)-pow(B,2))); pos += bytes;
		bytes = sprintf(content + pos, "<p>Type ordered pairs. Use commas to separate answers.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", A,B, A,-B, -A,B, -A,-B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", A,B, A,-B, -A,-B, -A,B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", A,B, -A,B, A,-B, -A,-B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", A,B, -A,B, -A,-B, A,-B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", A,B, -A,-B, -A,B, A,-B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", A,B, -A,-B, A,-B, -A,B); pos += bytes;
	
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", A,-B, A,B, -A,B, -A,-B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", A,-B, A,B, -A,-B, -A,B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", -A,B, A,B, A,-B, -A,-B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", -A,B, A,B, -A,-B, A,-B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", -A,-B, A,B, -A,B, A,-B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", -A,-B, A,B, A,-B, -A,B); pos += bytes;

		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", A,-B, -A,B, A,B, -A,-B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", A,-B, -A,-B, A,B, -A,B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", -A,B, A,-B, A,B, -A,-B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", -A,B, -A,-B, A,B, A,-B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", -A,-B, -A,B, A,B, A,-B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", -A,-B, A,-B, A,B, -A,B); pos += bytes;
	
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", A,-B, -A,B, -A,-B, A,B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", A,-B, -A,-B, -A,B, A,B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", -A,B, A,-B, -A,-B, A,B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", -A,B, -A,-B, A,-B, A,B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", -A,-B, -A,B, A,-B, A,B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)", -A,-B, A,-B, -A,B, A,B); pos += bytes;
	
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_5_10(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}
	
	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 9 + 1;
			B = rand() % 9 + 1;

			C = rand() % 8 + 2;
			D = rand() % 8 + 2;
		}
		while (A == B);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve the system of equations by the addition method.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\( \\begin{array}{ccccc} x^2 \\!\\! & - \\!\\! & %dy^2 \\!\\! & = \\!\\! & %d \\!\\! \\\\ ", C, (int)(pow(A,2)-(float)C*pow(B,2))); pos += bytes;
		bytes = sprintf(content + pos, "%dx^2 \\!\\! & + \\!\\! & y^2 \\!\\! & = \\!\\! & %d \\!\\! \\end{array} \\)</p>", D, (int)((float)D*pow(A,2)+pow(B,2))); pos += bytes;
		bytes = sprintf(content + pos, "<p>Type ordered pairs. Use commas to separate answers.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", A,B, A,-B, -A,B, -A,-B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", A,B, A,-B, -A,-B, -A,B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", A,B, -A,B, A,-B, -A,-B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", A,B, -A,B, -A,-B, A,-B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", A,B, -A,-B, -A,B, A,-B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", A,B, -A,-B, A,-B, -A,B); pos += bytes;
	
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", A,-B, A,B, -A,B, -A,-B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", A,-B, A,B, -A,-B, -A,B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", -A,B, A,B, A,-B, -A,-B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", -A,B, A,B, -A,-B, A,-B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", -A,-B, A,B, -A,B, A,-B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", -A,-B, A,B, A,-B, -A,B); pos += bytes;

		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", A,-B, -A,B, A,B, -A,-B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", A,-B, -A,-B, A,B, -A,B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", -A,B, A,-B, A,B, -A,-B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", -A,B, -A,-B, A,B, A,-B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", -A,-B, -A,B, A,B, A,-B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", -A,-B, A,-B, A,B, -A,B); pos += bytes;
	
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", A,-B, -A,B, -A,-B, A,B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", A,-B, -A,-B, -A,B, A,B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", -A,B, A,-B, -A,-B, A,B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", -A,B, -A,-B, A,-B, A,B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", -A,-B, -A,B, A,-B, A,B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)", -A,-B, A,-B, -A,B, A,B); pos += bytes;
	
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_5_11(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}
	
	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 9 + 1;
			B = rand() % 9 + 1;

			C = rand() % 8 + 2;
			D = rand() % 8 + 2;
			E = rand() % 8 + 2;
			F = rand() % 8 + 2;
		}
		while (A == B);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve the system of equations by the addition method.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\( \\begin{array}{ccccc} %dx^2 \\!\\! & + \\!\\! & %dy^2 \\!\\! & = \\!\\! & %d \\!\\! \\\\ ", C, D, (int)((float)C*pow(A,2)+(float)D*pow(B,2))); pos += bytes;
		bytes = sprintf(content + pos, "%dx^2 \\!\\! & - \\!\\! & %dy^2 \\!\\! & = \\!\\! & %d \\!\\! \\end{array} \\)</p>", E, F, (int)((float)E*pow(A,2)-(float)F*pow(B,2))); pos += bytes;
		bytes = sprintf(content + pos, "<p>Type ordered pairs. Use commas to separate answers.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", A,B, A,-B, -A,B, -A,-B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", A,B, A,-B, -A,-B, -A,B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", A,B, -A,B, A,-B, -A,-B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", A,B, -A,B, -A,-B, A,-B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", A,B, -A,-B, -A,B, A,-B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", A,B, -A,-B, A,-B, -A,B); pos += bytes;
	
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", A,-B, A,B, -A,B, -A,-B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", A,-B, A,B, -A,-B, -A,B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", -A,B, A,B, A,-B, -A,-B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", -A,B, A,B, -A,-B, A,-B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", -A,-B, A,B, -A,B, A,-B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", -A,-B, A,B, A,-B, -A,B); pos += bytes;

		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", A,-B, -A,B, A,B, -A,-B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", A,-B, -A,-B, A,B, -A,B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", -A,B, A,-B, A,B, -A,-B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", -A,B, -A,-B, A,B, A,-B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", -A,-B, -A,B, A,B, A,-B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", -A,-B, A,-B, A,B, -A,B); pos += bytes;
	
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", A,-B, -A,B, -A,-B, A,B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", A,-B, -A,-B, -A,B, A,B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", -A,B, A,-B, -A,-B, A,B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", -A,B, -A,-B, A,-B, A,B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)\t", -A,-B, -A,B, A,-B, A,B); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d,%d),(%d,%d),(%d,%d),(%d,%d)", -A,-B, A,-B, -A,B, A,B); pos += bytes;
	
		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_5_12(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 20 + 15;
			B = rand() % 20 + 40;
		}
		while (A >= B);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Find the length and width of a rectangle whose perimeter is %d feet and whose area is %d square feet.</p>", 
			2*A+2*B, A*B); pos += bytes;
		bytes = sprintf(content + pos, "<p>How many feet is the %s side?</p>", (randomizer%2==0?"longer":"shorter")); pos += bytes; 		

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos, "%d", (randomizer%2==0?B:A)); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_5_13(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	int num[3][4];

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 3 + 1;

			for (int i=0; i<3; i++)
			{
				for (int j=0; j<4; j++)
				{
					do
					{
						num[i][j] = rand() % 19 - 9;
					}
					while (fabs(num[i][j]) <= 1.0f);
				}
			}
		}
		while (A == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Write the equation represented by the %s row of the augmented matrix.</p>",
			(A==1?"1st":(A==2?"2nd":"3rd"))); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\( \\begin{bmatrix} "); pos += bytes;
		for (int i=0; i<3; i++)
		{
			bytes = sprintf(content + pos, "%d & %d & %d & | & %d \\\\ ", num[i][0], num[i][1], num[i][2], num[i][3]); pos += bytes;
		}
		bytes = sprintf(content + pos, "\\end{bmatrix} \\)</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>Use variables \\( x \\), \\( y \\), and \\( z \\) in that order.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%dx%c%dy%c%dz=%d", 
			num[A-1][0], 
			(num[A-1][1]>0?'+':'-'), (int)fabs(num[A-1][1]),
			(num[A-1][2]>0?'+':'-'), (int)fabs(num[A-1][2]),
			num[A-1][3]); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_5_14(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}
	
	float tx, ty;

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 18 + 2;
			B = rand() % 18 + 2;
			C = rand() % 38 + 2;
			D = rand() % 18 + 2;
			E = rand() % 18 + 2;
			F = rand() % 38 + 2;

			ty = (float)(F + (float)(C*D)/(float)A) / ((float)(-B*D)/(float)A-(float)E);
			tx = (float)(-C-B*ty)/(float)A;
		}
		while (A == 0 || B == 0 || C == 0 || D == 0 || E == 0 || F == 0 ||
			!isinteger(tx) || fabs(tx) > 20.0f || 
			!isinteger(ty) || fabs(ty) > 20.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve the system of equations using matrices.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\( \\begin{array}{ccccc} %dx \\!\\! & + \\!\\! & %dy \\!\\! & = \\!\\! & -%d \\!\\! \\\\ ", A, B, C); pos += bytes;
		bytes = sprintf(content + pos, "%dx \\!\\! & - \\!\\! & %dy \\!\\! & = \\!\\! & %d \\!\\! \\end{array} \\)</p>", D, E, F); pos += bytes;
		bytes = sprintf(content + pos, "<p>Type an ordered pair.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "(%d,%d)",
			(int)tx,
			(int)ty); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_5_15(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	float tx, ty, tz;

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 19 - 9;
			B = rand() % 19 - 9;
			C = rand() % 19 - 9;
			D = rand() % 19 - 9;
			E = rand() % 19 - 9;
			
			ty = (float)(B-C)/(float)(A+1);
			tz = (float)((float)(E-C)-2.0f*ty)/(float)(D+1);
			tx = (float)(C)+ty+tz;
		}
		while (fabs(A) <= 1.0f || B == 0 || C == 0 || fabs(D) <= 1.0f || E == 0 ||
			!isinteger(tx) || !isinteger(ty) || !isinteger(tz));

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve the system of equations using matrices.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\( \\begin{array}{l} "); pos += bytes;

		bytes = sprintf(content + pos, "x%c%dy=z%c%d \\\\ ", (A>0?'+':'-'), (int)fabs(A), (B>0?'+':'-'), (int)fabs(B)); pos += bytes;
		bytes = sprintf(content + pos, "x=%d+y+z \\\\ ", C); pos += bytes;
		bytes = sprintf(content + pos, "x+y%c%dz=%d \\\\ ", (D>0?'+':'-'), (int)fabs(D), E); pos += bytes;

		bytes = sprintf(content + pos, "\\end{array} \\)</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>Type an ordered triplet.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "(%d,%d,%d)", (int)tx, (int)ty, (int)tz); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_5_16(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	float tx, ty, tz;

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 19 - 9;
			B = rand() % 19 - 9;
			C = rand() % 19 - 9;
			D = rand() % 39 - 19;
			E = rand() % 19 - 9;
			F = rand() % 19 - 9;
			G = rand() % 19 - 9;
			H = rand() % 39 - 19;
			I = rand() % 19 - 9;
			J = rand() % 19 - 9;
			K = rand() % 19 - 9;
			L = rand() % 39 - 19;

			if (A == 0) { }
			else if (F-B*E/A == 0) { A = 0; }
			else if (K-C*I/A == 0) { A = 0; }
			else if ((B*I/A-J)*(C*E/A-G)/((B*E/A-F)*(K-C*I/A))+1 == 0) { A = 0; }
			else
			{
				tz = (float)(L-D*I/(float)A+(B*I/(float)A-J)*(H-D*E/(float)A)/(float)(F-B*E/(float)A))/(float)(K-C*I/(float)A)/
					(float)((B*I/(float)A-J)*(C*E/(float)A-G)/(float)((B*E/(float)A-F)*(K-C*I/(float)A))+1.0f);
				ty = (float)(H-D*E/(float)A+(float)(C*E/(float)A-G)*tz)/(float)(F-B*E/(float)A);
				tx = (float)(D-B*ty-C*tz)/(float)A;
			}

			//tz = (float)((L-(float)(D*I)/(float)A+(float)((float)(B*I)/(float)A-(float)J)*((float)(H-(float)(D*E)/(float)A)/(float)(F-(float)(B*E)/(float)A)))/
			//	(float)((float)K-(float)(C*I)/(float)A))/
			//	(float)((((float)(B*I)/(float)A-(float)J)*((float)(B*E)/(float)A-(float)G))/((float)(B*E)/(float)A-(float)F)+1.0f);
			//ty = (float)((float)H-(float)(D*E)/(float)A+(float)((float)(B*E)/(float)A-(float)G)*tz)/((float)F-(float)(B*E)/(float)A);
			//tx = (float)((float)D-(float)B*ty-(float)C*tz)/(float)A;
		}
		while (A == 0 || fabs(A) <= 1.0f || fabs(B) <= 1.0f || fabs(C) <= 1.0f ||
			fabs(E) <= 1.0f || fabs(F) <= 1.0f || fabs(G) <= 1.0f ||
			fabs(I) <= 1.0f || fabs(J) <= 1.0f || fabs(K) <= 1.0f ||
			!isinteger(tx) || !isinteger(ty) || !isinteger(tz) ||
			fabs(tx) >= 50.0f || fabs(ty) >= 50.0f || fabs(tz) >= 50.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve the system of equations using matrices.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\( \\begin{array}{l} "); pos += bytes;

		bytes = sprintf(content + pos, "%dx%c%dy%c%dz=%d \\\\ ", A, (B>0?'+':'-'), (int)fabs(B), (C>0?'+':'-'), (int)fabs(C), D); pos += bytes;
		bytes = sprintf(content + pos, "%dx%c%dy%c%dz=%d \\\\ ", E, (F>0?'+':'-'), (int)fabs(F), (G>0?'+':'-'), (int)fabs(G), H); pos += bytes;
		bytes = sprintf(content + pos, "%dx%c%dy%c%dz=%d \\\\ ", I, (J>0?'+':'-'), (int)fabs(J), (K>0?'+':'-'), (int)fabs(K), L); pos += bytes;

		bytes = sprintf(content + pos, "\\end{array} \\)</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>Type an ordered triplet.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "(%d,%d,%d)", (int)tx, (int)ty, (int)tz); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_5_17(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	float tx, ty, tz;
	int r;

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			r = rand() % 4 + 2;

			A = 1;
			B = 1;
			C = 1;
			D = rand() % 199 - 100;
			E = (int)pow(r,2);
			F = -r;
			G = 1;
			H = rand() % 199 - 100;
			I = (int)pow(r,2);
			J = r;
			K = 1;
			L = rand() % 199 - 100;

			if (A == 0) { }
			else if (F-B*E/A == 0) { A = 0; }
			else if (K-C*I/A == 0) { A = 0; }
			else if ((B*I/A-J)*(C*E/A-G)/((B*E/A-F)*(K-C*I/A))+1 == 0) { A = 0; }
			else
			{
				tz = (float)(L-D*I/(float)A+(B*I/(float)A-J)*(H-D*E/(float)A)/(float)(F-B*E/(float)A))/(float)(K-C*I/(float)A)/
					(float)((B*I/(float)A-J)*(C*E/(float)A-G)/(float)((B*E/(float)A-F)*(K-C*I/(float)A))+1.0f);
				ty = (float)(H-D*E/(float)A+(float)(C*E/(float)A-G)*tz)/(float)(F-B*E/(float)A);
				tx = (float)(D-B*ty-C*tz)/(float)A;
			}
		}
		while (A == 0 ||
			!isinteger(tx) || !isinteger(ty) || !isinteger(tz) ||
			fabs(tx) <= 1.0f || fabs(ty) <= 1.0f || fabs(tz) <= 1.0f ||
			fabs(tx) >= 50.0f || fabs(ty) >= 50.0f || fabs(tz) >= 50.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Find the quadratic function \\( f(x) = ax^{2} + bx + c \\) for which:</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\( \\begin{array}{l} "); pos += bytes;

		bytes = sprintf(content + pos, "f(%d)=%d \\\\ ", 1, D); pos += bytes;
		bytes = sprintf(content + pos, "f(%d)=%d \\\\ ", -r, H); pos += bytes;
		bytes = sprintf(content + pos, "f(%d)=%d \\\\ ", r, L); pos += bytes;

		bytes = sprintf(content + pos, "\\end{array} \\)</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>The function \\( f(x) \\ = \\ \\ ? \\)</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%dx{^{`2`}}%c%dx%c%d\t", (int)tx, ((int)ty>0?'+':'-'), (int)fabs(ty), ((int)tz>0?'+':'-'), (int)fabs(tz)); pos += bytes;
		bytes = sprintf(answers + pos,  "%d{x^{`2`}}%c%d%c%dx\t", (int)tx, ((int)tz>0?'+':'-'), (int)fabs(tz), ((int)tx>0?'+':'-'), (int)fabs(tx)); pos += bytes;
		bytes = sprintf(answers + pos,  "%dx%c%dx{^{`2`}}%c%d\t", (int)ty, ((int)tx>0?'+':'-'), (int)fabs(tx), ((int)tz>0?'+':'-'), (int)fabs(tz)); pos += bytes;
		bytes = sprintf(answers + pos,  "%d%c%dx{^{`2`}}%c%dx\t", (int)tz, ((int)tx>0?'+':'-'), (int)fabs(tx), ((int)ty>0?'+':'-'), (int)fabs(ty)); pos += bytes;
		bytes = sprintf(answers + pos,  "%dx%c%d%c%dx{^{`2`}}\t", (int)ty, ((int)tz>0?'+':'-'), (int)fabs(tz), ((int)tx>0?'+':'-'), (int)fabs(tx)); pos += bytes;
		bytes = sprintf(answers + pos,  "%d%c%dx%c%dx{^{`2`}}", (int)tz, ((int)ty>0?'+':'-'), (int)fabs(ty), ((int)tx>0?'+':'-'), (int)fabs(tx)); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;


	return 1;
};

int question_5_18(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	float tx, ty, tz;

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = (rand() % 5 + 1) * 100;
			B = (rand() % 5 + 1) * 100;
			C = (rand() % 5 + 1) * 100;
			D = (rand() % 50 + 1) * 100;
			E = (rand() % 20 + 1);
			F = (rand() % 20 + 1);
			G = (rand() % 20 + 1);
			H = (rand() % 200 + 1);
			I = (rand() % 20 + 1) * 10;
			J = (rand() % 20 + 1) * 10;
			K = (rand() % 20 + 1) * 10;
			L = (rand() % 200 + 1) * 10;

			if (A == 0) { }
			else if (F-B*E/A == 0) { A = 0; }
			else if (K-C*I/A == 0) { A = 0; }
			else if ((B*I/A-J)*(C*E/A-G)/((B*E/A-F)*(K-C*I/A))+1 == 0) { A = 0; }
			else
			{
				tz = (float)(L-D*I/(float)A+(B*I/(float)A-J)*(H-D*E/(float)A)/(float)(F-B*E/(float)A))/(float)(K-C*I/(float)A)/
					(float)((B*I/(float)A-J)*(C*E/(float)A-G)/(float)((B*E/(float)A-F)*(K-C*I/(float)A))+1.0f);
				ty = (float)(H-D*E/(float)A+(float)(C*E/(float)A-G)*tz)/(float)(F-B*E/(float)A);
				tx = (float)(D-B*ty-C*tz)/(float)A;
			}
		}
		while (A == 0 || fabs(A) <= 1.0f || fabs(B) <= 1.0f || fabs(C) <= 1.0f ||
			fabs(E) <= 1.0f || fabs(F) <= 1.0f || fabs(G) <= 1.0f ||
			fabs(I) <= 1.0f || fabs(J) <= 1.0f || fabs(K) <= 1.0f ||
			!isinteger(tx) || !isinteger(ty) || !isinteger(tz) ||
			tx <= 0.0f || ty <= 0.0f || tz <= 0.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>The nutritional content per ounce in calories, protein, and vitamin C, of three foods is presented in the table below.</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p><table border='1'><tr><td></td><td>Cal</td><td>Pro</td><td>Vit&nbsp;C</td></tr>"); pos += bytes;
		bytes = sprintf(content + pos, "<tr><td>Food&nbsp;A</td><td>%d</td><td>%d</td><td>%d</td></tr>", A, E, I); pos += bytes;
		bytes = sprintf(content + pos, "<tr><td>Food&nbsp;B</td><td>%d</td><td>%d</td><td>%d</td></tr>", B, F, J); pos += bytes;
		bytes = sprintf(content + pos, "<tr><td>Food&nbsp;C</td><td>%d</td><td>%d</td><td>%d</td></tr></table></p>", C, G, K); pos += bytes;

		//bytes = sprintf(content + pos, "<p>\\( \\begin{array}{c|c|c|c} "); pos += bytes;
		//bytes = sprintf(content + pos, " & \\text{Calories} & \\text{Protein} & \\text{Vitamin C} \\\\ "); pos += bytes;
		//bytes = sprintf(content + pos, "\\text{Food A} & %d & %d & %d \\\\ ", A, E, I); pos += bytes;
		//bytes = sprintf(content + pos, "\\text{Food B} & %d & %d & %d \\\\ ", B, F, J); pos += bytes;
		//bytes = sprintf(content + pos, "\\text{Food C} & %d & %d & %d \\\\ ", C, G, K); pos += bytes;
		//bytes = sprintf(content + pos, "\\end{array} \\)</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p>If a meal of these three foods has exactly %d calories, %d protein, and %d vitamin C, ",
			D, H, L); pos += bytes;
		bytes = sprintf(content + pos, " how many ounces of Food %s should be used?</p>", (randomizer%3==0?"A":(randomizer%3==1?"B":"C"))); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%d", (randomizer%3==0?(int)tx:(randomizer%3==1?(int)ty:(int)tz))); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_5_19(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 19 - 9;
			B = rand() % 19 - 9;
			C = rand() % 19 - 9;
			D = rand() % 39 - 19;
			E = rand() % 19 - 9;
			F = rand() % 19 - 9;
			G = rand() % 19 - 9;
			H = rand() % 39 - 19;
			I = rand() % 19 - 9;
			J = rand() % 19 - 9;
			K = rand() % 19 - 9;
			L = rand() % 39 - 19;

			if (A == 0) { }
			else if (F-B*E/A == 0) { A = 0; }
			else if (K-C*I/A == 0) { A = 0; }
			else if ((float)(B*I/(float)A-J)*(float)(C*E/(float)A-G)/(float)((float)(B*E/(float)A-F)*(float)(K-C*I/(float)A))+1.0f != 0.0f) { A = 0; }
		}
		while (A == 0 || fabs(A) <= 1.0f || fabs(B) <= 1.0f || fabs(C) <= 1.0f ||
			fabs(E) <= 1.0f || fabs(F) <= 1.0f || fabs(G) <= 1.0f ||
			fabs(I) <= 1.0f || fabs(J) <= 1.0f || fabs(K) <= 1.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve the system of equations using matrices.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\( \\begin{array}{l} "); pos += bytes;

		bytes = sprintf(content + pos, "%dx%c%dy%c%dz=%d \\\\ ", A, (B>0?'+':'-'), (int)fabs(B), (C>0?'+':'-'), (int)fabs(C), D); pos += bytes;
		bytes = sprintf(content + pos, "%dx%c%dy%c%dz=%d \\\\ ", E, (F>0?'+':'-'), (int)fabs(F), (G>0?'+':'-'), (int)fabs(G), H); pos += bytes;
		bytes = sprintf(content + pos, "%dx%c%dy%c%dz=%d \\\\ ", I, (J>0?'+':'-'), (int)fabs(J), (K>0?'+':'-'), (int)fabs(K), L); pos += bytes;

		bytes = sprintf(content + pos, "\\end{array} \\)</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>Type an ordered triplet, use \\( z \\) as the independent variable. If the system has no solution, put \\( d \\) for 'does not exist'.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "d"); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;


	return 1;
};

int question_5_20(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	float tx1, ty1, tx2, ty2;
	int r1, r2;

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			r1 = rand() % 9 + 1;
			r2 = rand() % 9 + 1;

			A = rand() % 19 - 9;
			B = rand() % 19 - 9;
			C = rand() % 19 - 9;
			D = rand() % 39 - 19;
			E = rand() % 19 - 9;
			F = rand() % 19 - 9;
			G = rand() % 19 - 9;
			H = rand() % 39 - 19;
			I = A * r1 + E * r2;
			J = B * r1 + F * r2;
			K = C * r1 + G * r2;
			L = D * r1 + H * r2;

			if (A == 0) { }
			else if (F-B*E/A == 0) { A = 0; }
			else
			{
				ty1 = (float)(H-D*E/(float)A)/(float)(F-B*E/(float)A);
				ty2 = -(float)(G-C*E/(float)A)/(float)(F-B*E/(float)A);

				tx1 = (float)D/(float)A-(float)B/(float)A*((float)(H-D*E/(float)A)/(float)(F-B*E/(float)A));
				tx2 = 1.0f/(float)A*(float)((float)-C+(float)B*(float)(G-C*E/(float)A)/(float)(F-B*E/(float)A)); 
			}
		}
		while (A == 0 || fabs(A) <= 1.0f || fabs(B) <= 1.0f || fabs(C) <= 1.0f ||
			fabs(E) <= 1.0f || fabs(F) <= 1.0f || fabs(G) <= 1.0f ||
			fabs(I) <= 1.0f || fabs(J) <= 1.0f || fabs(K) <= 1.0f ||
			!isinteger(tx1) || !isinteger(ty1) || 
			!isinteger(tx2) || !isinteger(ty2) ||
			fabs(tx1) <= 1.0f || fabs(ty1) <= 1.0f ||
			fabs(tx2) <= 1.0f || fabs(ty2) <= 1.0f ||
			fabs(tx1) >= 50.0f || fabs(ty1) >= 50.0f || 
			fabs(tx2) >= 50.0f || fabs(ty2) >= 50.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve the system of equations using matrices.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\( \\begin{array}{l} "); pos += bytes;

		bytes = sprintf(content + pos, "%dx%c%dy%c%dz=%d \\\\ ", A, (B>0?'+':'-'), (int)fabs(B), (C>0?'+':'-'), (int)fabs(C), D); pos += bytes;
		bytes = sprintf(content + pos, "%dx%c%dy%c%dz=%d \\\\ ", E, (F>0?'+':'-'), (int)fabs(F), (G>0?'+':'-'), (int)fabs(G), H); pos += bytes;
		bytes = sprintf(content + pos, "%dx%c%dy%c%dz=%d \\\\ ", I, (J>0?'+':'-'), (int)fabs(J), (K>0?'+':'-'), (int)fabs(K), L); pos += bytes;

		bytes = sprintf(content + pos, "\\end{array} \\)</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>Type an ordered triplet, use \\( z \\) as the independent variable. If the system has no solution, put \\( d \\) for 'does not exist'.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "(%d%c%dz,%d%c%dz,z)\t", (int)tx1, ((int)tx2>0?'+':'-'), (int)fabs(tx2), (int)ty1, ((int)ty2>0?'+':'-'), (int)fabs(ty2)); pos += bytes;
		bytes = sprintf(answers + pos,  "(%dz%c%d,%d%c%dz,z)\t", (int)tx2, ((int)tx1>0?'+':'-'), (int)fabs(tx1), (int)ty1, ((int)ty2>0?'+':'-'), (int)fabs(ty2)); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d%c%dz,%dz%c%d,z)\t", (int)tx1, ((int)tx2>0?'+':'-'), (int)fabs(tx2), (int)ty2, ((int)ty1>0?'+':'-'), (int)fabs(ty1)); pos += bytes;
		bytes = sprintf(answers + pos,  "(%dz%c%d,%dz%c%d,z)", (int)tx2, ((int)tx1>0?'+':'-'), (int)fabs(tx1), (int)ty2, ((int)ty1>0?'+':'-'), (int)fabs(ty1)); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_5_21(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	float tx1, ty1, tx2, ty2;
	int r1, r2;

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			r1 = rand() % 9 + 1;
			r2 = rand() % 9 + 1;

			A = rand() % 19 - 9;
			B = rand() % 19 - 9;
			C = rand() % 19 - 9;
			D = rand() % 39 - 19;
			E = rand() % 19 - 9;
			F = rand() % 19 - 9;
			G = rand() % 19 - 9;
			H = rand() % 39 - 19;
			I = A * r1 - E * r2;
			J = B * r1 - F * r2;
			K = C * r1 - G * r2;
			L = D * r1 - H * r2;

			if (A == 0) { }
			else if (F-B*E/A == 0) { A = 0; }
			else
			{
				ty1 = (float)(H-D*E/(float)A)/(float)(F-B*E/(float)A);
				ty2 = -(float)(G-C*E/(float)A)/(float)(F-B*E/(float)A);

				tx1 = (float)D/(float)A-(float)B/(float)A*((float)(H-D*E/(float)A)/(float)(F-B*E/(float)A));
				tx2 = 1.0f/(float)A*(float)((float)-C+(float)B*(float)(G-C*E/(float)A)/(float)(F-B*E/(float)A)); 
			}
		}
		while (A == 0 || fabs(A) <= 1.0f || fabs(B) <= 1.0f || fabs(C) <= 1.0f ||
			fabs(E) <= 1.0f || fabs(F) <= 1.0f || fabs(G) <= 1.0f ||
			fabs(I) <= 1.0f || fabs(J) <= 1.0f || fabs(K) <= 1.0f ||
			!isinteger(tx1) || !isinteger(ty1) || 
			!isinteger(tx2) || !isinteger(ty2) ||
			fabs(tx1) <= 1.0f || fabs(ty1) <= 1.0f ||
			fabs(tx2) <= 1.0f || fabs(ty2) <= 1.0f ||
			fabs(tx1) >= 50.0f || fabs(ty1) >= 50.0f || 
			fabs(tx2) >= 50.0f || fabs(ty2) >= 50.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve the system of equations using matrices.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\( \\begin{array}{l} "); pos += bytes;

		bytes = sprintf(content + pos, "%dx%c%dy%c%dz=%d \\\\ ", A, (B>0?'+':'-'), (int)fabs(B), (C>0?'+':'-'), (int)fabs(C), D); pos += bytes;
		bytes = sprintf(content + pos, "%dx%c%dy%c%dz=%d \\\\ ", E, (F>0?'+':'-'), (int)fabs(F), (G>0?'+':'-'), (int)fabs(G), H); pos += bytes;
		bytes = sprintf(content + pos, "%dx%c%dy%c%dz=%d \\\\ ", I, (J>0?'+':'-'), (int)fabs(J), (K>0?'+':'-'), (int)fabs(K), L); pos += bytes;

		bytes = sprintf(content + pos, "\\end{array} \\)</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>Type an ordered triplet, use \\( z \\) as the independent variable. If the system has no solution, put \\( d \\) for 'does not exist'.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "(%d%c%dz,%d%c%dz,z)\t", (int)tx1, ((int)tx2>0?'+':'-'), (int)fabs(tx2), (int)ty1, ((int)ty2>0?'+':'-'), (int)fabs(ty2)); pos += bytes;
		bytes = sprintf(answers + pos,  "(%dz%c%d,%d%c%dz,z)\t", (int)tx2, ((int)tx1>0?'+':'-'), (int)fabs(tx1), (int)ty1, ((int)ty2>0?'+':'-'), (int)fabs(ty2)); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d%c%dz,%dz%c%d,z)\t", (int)tx1, ((int)tx2>0?'+':'-'), (int)fabs(tx2), (int)ty2, ((int)ty1>0?'+':'-'), (int)fabs(ty1)); pos += bytes;
		bytes = sprintf(answers + pos,  "(%dz%c%d,%dz%c%d,z)", (int)tx2, ((int)tx1>0?'+':'-'), (int)fabs(tx1), (int)ty2, ((int)ty1>0?'+':'-'), (int)fabs(ty1)); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_5_22(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	float tx1, ty1, tx2, ty2;
	int r1, r2;

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			r1 = rand() % 9 + 1;
			r2 = rand() % 9 + 1;

			A = rand() % 19 - 9;
			B = rand() % 19 - 9;
			C = rand() % 19 - 9;
			D = rand() % 39 - 19;
			E = rand() % 19 - 9;
			F = rand() % 19 - 9;
			G = rand() % 19 - 9;
			H = rand() % 39 - 19;
			I = -A * r1 + E * r2;
			J = -B * r1 + F * r2;
			K = -C * r1 + G * r2;
			L = -D * r1 + H * r2;

			if (A == 0) { }
			else if (F-B*E/A == 0) { A = 0; }
			else
			{
				ty1 = (float)(H-D*E/(float)A)/(float)(F-B*E/(float)A);
				ty2 = -(float)(G-C*E/(float)A)/(float)(F-B*E/(float)A);

				tx1 = (float)D/(float)A-(float)B/(float)A*((float)(H-D*E/(float)A)/(float)(F-B*E/(float)A));
				tx2 = 1.0f/(float)A*(float)((float)-C+(float)B*(float)(G-C*E/(float)A)/(float)(F-B*E/(float)A)); 
			}
		}
		while (A == 0 || fabs(A) <= 1.0f || fabs(B) <= 1.0f || fabs(C) <= 1.0f ||
			fabs(E) <= 1.0f || fabs(F) <= 1.0f || fabs(G) <= 1.0f ||
			fabs(I) <= 1.0f || fabs(J) <= 1.0f || fabs(K) <= 1.0f ||
			!isinteger(tx1) || !isinteger(ty1) || 
			!isinteger(tx2) || !isinteger(ty2) ||
			fabs(tx1) <= 1.0f || fabs(ty1) <= 1.0f ||
			fabs(tx2) <= 1.0f || fabs(ty2) <= 1.0f ||
			fabs(tx1) >= 50.0f || fabs(ty1) >= 50.0f || 
			fabs(tx2) >= 50.0f || fabs(ty2) >= 50.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve the system of equations using matrices.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\( \\begin{array}{l} "); pos += bytes;

		bytes = sprintf(content + pos, "%dx%c%dy%c%dz=%d \\\\ ", A, (B>0?'+':'-'), (int)fabs(B), (C>0?'+':'-'), (int)fabs(C), D); pos += bytes;
		bytes = sprintf(content + pos, "%dx%c%dy%c%dz=%d \\\\ ", E, (F>0?'+':'-'), (int)fabs(F), (G>0?'+':'-'), (int)fabs(G), H); pos += bytes;

		bytes = sprintf(content + pos, "\\end{array} \\)</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>Type an ordered triplet, use \\( z \\) as the independent variable. If the system has no solution, put \\( d \\) for 'does not exist'.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "(%d%c%dz,%d%c%dz,z)\t", (int)tx1, ((int)tx2>0?'+':'-'), (int)fabs(tx2), (int)ty1, ((int)ty2>0?'+':'-'), (int)fabs(ty2)); pos += bytes;
		bytes = sprintf(answers + pos,  "(%dz%c%d,%d%c%dz,z)\t", (int)tx2, ((int)tx1>0?'+':'-'), (int)fabs(tx1), (int)ty1, ((int)ty2>0?'+':'-'), (int)fabs(ty2)); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d%c%dz,%dz%c%d,z)\t", (int)tx1, ((int)tx2>0?'+':'-'), (int)fabs(tx2), (int)ty2, ((int)ty1>0?'+':'-'), (int)fabs(ty1)); pos += bytes;
		bytes = sprintf(answers + pos,  "(%dz%c%d,%dz%c%d,z)", (int)tx2, ((int)tx1>0?'+':'-'), (int)fabs(tx1), (int)ty2, ((int)ty1>0?'+':'-'), (int)fabs(ty1)); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_5_23(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 39 - 19;
			B = rand() % 39 - 19;
			C = rand() % 39 - 19;
			D = rand() % 39 - 19;
			E = rand() % 39 - 19;
			F = rand() % 39 - 19;
		}
		while (A == 0 || fabs(A) <= 1.0f || fabs(B) <= 1.0f || fabs(C) <= 1.0f ||
			fabs(E) <= 1.0f || fabs(F) <= 1.0f || F-E != -B+A-D+C ||
			-B+A-D+C == 0 || -D+C == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Solve the system of equations using matrices.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>\\( \\begin{array}{l} "); pos += bytes;

		bytes = sprintf(content + pos, "x%c%d=y%c%d \\\\ ", (A>0?'+':'-'), (int)fabs(A), (B>0?'+':'-'), (int)fabs(B)); pos += bytes;
		bytes = sprintf(content + pos, "z%c%d=x%c%d \\\\ ", (C>0?'+':'-'), (int)fabs(C), (D>0?'+':'-'), (int)fabs(D)); pos += bytes;
		bytes = sprintf(content + pos, "y%c%d=z%c%d \\\\ ", (E>0?'+':'-'), (int)fabs(E), (F>0?'+':'-'), (int)fabs(F)); pos += bytes;

		bytes = sprintf(content + pos, "\\end{array} \\)</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p>Type an ordered triplet, use \\( z \\) as the independent variable. If the system has no solution, put \\( d \\) for 'does not exist'.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "(%d+z,%d+z,z)\t", -D+C, -B+A-D+C); pos += bytes;
		bytes = sprintf(answers + pos,  "(z%c%d,%d+z,z)\t", (-D+C>0?'+':'-'), (int)fabs(-D+C), -B+A-D+C); pos += bytes;
		bytes = sprintf(answers + pos,  "(%d+z,z%c%d,z)\t", -D+C, (-B+A-D+C>0?'+':'-'), (int)fabs(-B+A-D+C)); pos += bytes;
		bytes = sprintf(answers + pos,  "(z%c%d,z%c%d,z)", (-D+C>0?'+':'-'), (int)fabs(-D+C), (-B+A-D+C>0?'+':'-'), (int)fabs(-B+A-D+C)); pos += bytes;
		

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_5_24(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 39 - 19;
			B = rand() % 39 - 19;
			C = rand() % 39 - 19;
			D = rand() % 39 - 19;
			E = rand() % 39 - 19;
			F = rand() % 39 - 19;
			G = rand() % 39 - 19;
			H = rand() % 39 - 19;
			I = rand() % 19 - 9;
			J = rand() % 19 - 9;
		}
		while (A == 0 || fabs(I) <= 1.0f || fabs(J) <= 1.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		if (randomizer% 4 == 0)
		{
			bytes = sprintf(content + pos, "<p>Find \\( A + B \\) using the matrices given.</p>"); pos += bytes;
		}
		else if (randomizer% 4 == 1)
		{
			bytes = sprintf(content + pos, "<p>Find \\( A - B \\) using the matrices given.</p>"); pos += bytes;
		}
		else if (randomizer% 4 == 2)
		{
			bytes = sprintf(content + pos, "<p>Find \\( %dA \\) using the matrices given.</p>", I); pos += bytes;
		}
		else if (randomizer% 4 == 3)
		{
			bytes = sprintf(content + pos, "<p>Find \\( %dA%c%dB \\) using the matrices given.</p>", I, (J>0?'+':'-'), (int)fabs(J)); pos += bytes;
		}

		bytes = sprintf(content + pos, "<p>\\( A \\ = \\ \\begin{bmatrix} "); pos += bytes;
		bytes = sprintf(content + pos, "%d & %d \\\\ ", A, B); pos += bytes;
		bytes = sprintf(content + pos, "%d & %d \\\\ ", C, D); pos += bytes;
		bytes = sprintf(content + pos, "\\end{bmatrix} \\)</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p>\\( B \\ = \\ \\begin{bmatrix} "); pos += bytes;
		bytes = sprintf(content + pos, "%d & %d \\\\ ", E, F); pos += bytes;
		bytes = sprintf(content + pos, "%d & %d \\\\ ", G, H); pos += bytes;
		bytes = sprintf(content + pos, "\\end{bmatrix} \\)</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p>Type a matrix.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		if (randomizer% 4 == 0)
		{
			bytes = sprintf(answers + pos,  "{\\begin{bmatrix}{`%d`}&{`%d`}\\\\{`%d`}&{`%d`}\\end{bmatrix}}", A+E, B+F, C+G, D+H); pos += bytes;	
		}
		else if (randomizer% 4 == 1)
		{
			bytes = sprintf(answers + pos,  "{\\begin{bmatrix}{`%d`}&{`%d`}\\\\{`%d`}&{`%d`}\\end{bmatrix}}", A-E, B-F, C-G, D-H); pos += bytes;	
		}
		else if (randomizer% 4 == 2)
		{
			bytes = sprintf(answers + pos,  "{\\begin{bmatrix}{`%d`}&{`%d`}\\\\{`%d`}&{`%d`}\\end{bmatrix}}", I*A, I*B, I*C, I*D); pos += bytes;	
		}
		else if (randomizer% 4 == 3)
		{
			bytes = sprintf(answers + pos,  "{\\begin{bmatrix}{`%d`}&{`%d`}\\\\{`%d`}&{`%d`}\\end{bmatrix}}", I*A+J*E, I*B+J*F, I*C+J*G, I*D+J*H); pos += bytes;	
		}	

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}

		// {\begin{bmatrix}{{3}{7}}&{{-}{2}{0}}\\{{1}}&{{1}{8}}\end{bmatrix}}
		// {\\begin{bmatrix}{{3}{7}}&{{-}{2}{0}}\\\\{{1}}&{{1}{8}}\\end{bmatrix}}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_5_25(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	int other[9];

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 39 - 19;
			B = rand() % 39 - 19;
			C = rand() % 39 - 19;
			D = rand() % 39 - 19;
			E = rand() % 39 - 19;
			F = rand() % 39 - 19;
			G = rand() % 39 - 19;
			H = rand() % 39 - 19;
			I = rand() % 39 - 19;
			J = rand() % 19 - 9;

			for (int i=0; i<9; i++) other[i] = rand() % 39 - 19;
		}
		while (A == 0 || fabs(J) <= 1.0f);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Use the given matrices to solve for \\( X \\).</p>"); pos += bytes;
		

		bytes = sprintf(content + pos, "<p>\\( A \\ = \\ \\begin{bmatrix} "); pos += bytes;
		bytes = sprintf(content + pos, "%d & %d & %d \\\\ ", A, B, C); pos += bytes;
		bytes = sprintf(content + pos, "%d & %d & %d \\\\ ", D, E, F); pos += bytes;
		bytes = sprintf(content + pos, "%d & %d & %d \\\\ ", G, H, I); pos += bytes;
		bytes = sprintf(content + pos, "\\end{bmatrix} \\)</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p>\\( B \\ = \\ \\begin{bmatrix} "); pos += bytes;
		bytes = sprintf(content + pos, "%d & %d & %d \\\\ ", other[0], other[1], other[2]); pos += bytes;
		bytes = sprintf(content + pos, "%d & %d & %d \\\\ ", other[3], other[4], other[5]); pos += bytes;
		bytes = sprintf(content + pos, "%d & %d & %d \\\\ ", other[6], other[7], other[8]); pos += bytes;
		bytes = sprintf(content + pos, "\\end{bmatrix} \\)</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p>\\( B - X = %dA, \\ X \\ = \\ ? \\)</p>", J); pos += bytes;

		bytes = sprintf(content + pos, "<p>Type a matrix.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "{\\begin{bmatrix}{`%d`}&{`%d`}&{`%d`}\\\\{`%d`}&{`%d`}&{`%d`}\\\\{`%d`}&{`%d`}&{`%d`}\\end{bmatrix}}", 
			other[0]-J*A, other[1]-J*B, other[2]-J*C, 
			other[3]-J*D, other[4]-J*E, other[5]-J*F, 
			other[6]-J*G, other[7]-J*H, other[8]-J*I); pos += bytes;	

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_5_26(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 39 - 19;
			B = rand() % 39 - 19;
			C = rand() % 39 - 19;
			D = rand() % 39 - 19;
			E = rand() % 39 - 19;
			F = rand() % 39 - 19;
			G = rand() % 39 - 19;
			H = rand() % 39 - 19;
			I = rand() % 39 - 19;
			J = rand() % 39 - 19;
			K = rand() % 39 - 19;
			L = rand() % 39 - 19;
		}
		while (A == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		if (randomizer% 2 == 0)
		{
			bytes = sprintf(content + pos, "<p>Given the following matrices, find AB.</p>"); pos += bytes;
		}
		else if (randomizer% 2 == 1)
		{
			bytes = sprintf(content + pos, "<p>Given the following matrices, find BA.</p>"); pos += bytes;
		}
		
		bytes = sprintf(content + pos, "<p>\\( A \\ = \\ \\begin{bmatrix} "); pos += bytes;
		bytes = sprintf(content + pos, "%d & %d & %d \\\\ ", A, B, C); pos += bytes;
		bytes = sprintf(content + pos, "%d & %d & %d \\\\ ", D, E, F); pos += bytes;
		bytes = sprintf(content + pos, "\\end{bmatrix} \\)</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p>\\( B \\ = \\ \\begin{bmatrix} "); pos += bytes;
		bytes = sprintf(content + pos, "%d & %d \\\\ ", G, H); pos += bytes;
		bytes = sprintf(content + pos, "%d & %d \\\\ ", I, J); pos += bytes;
		bytes = sprintf(content + pos, "%d & %d \\\\ ", K, L); pos += bytes;
		bytes = sprintf(content + pos, "\\end{bmatrix} \\)</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p>Type a matrix.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		if (randomizer% 2 == 0)
		{
			bytes = sprintf(answers + pos,  "{\\begin{bmatrix}{`%d`}&{`%d`}\\\\{`%d`}&{`%d`}\\end{bmatrix}}", 
				A*G+B*I+C*K, A*H+B*J+C*L, D*G+E*I+F*K, D*H+E*J+F*L); pos += bytes;	
		}
		else if (randomizer% 2 == 1)
		{
			bytes = sprintf(answers + pos,  "{\\begin{bmatrix}{`%d`}&{`%d`}&{`%d`}\\\\{`%d`}&{`%d`}&{`%d`}\\\\{`%d`}&{`%d`}&{`%d`}\\end{bmatrix}}", 
				G*A+H*D, G*B+H*E, G*C+H*F,
				I*A+J*D, I*B+J*E, I*C+J*F,
				K*A+L*D, K*B+L*E, K*C+L*F); pos += bytes;	
		}

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_5_27(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 39 - 19;
			B = rand() % 39 - 19;
			C = rand() % 39 - 19;
			D = rand() % 39 - 19;
			E = rand() % 39 - 19;
			F = rand() % 39 - 19;
			G = rand() % 39 - 19;
			H = rand() % 39 - 19;
			I = rand() % 39 - 19;
			J = rand() % 39 - 19;
			K = rand() % 39 - 19;
			L = rand() % 39 - 19;
		}
		while (A == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Given the following matrices, find A(BC).</p>"); pos += bytes;
		
		bytes = sprintf(content + pos, "<p>\\( A \\ = \\ \\begin{bmatrix} "); pos += bytes;
		bytes = sprintf(content + pos, "%d & %d \\\\ ", A, B); pos += bytes;
		bytes = sprintf(content + pos, "%d & %d \\\\ ", C, D); pos += bytes;
		bytes = sprintf(content + pos, "\\end{bmatrix} \\)</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p>\\( B \\ = \\ \\begin{bmatrix} "); pos += bytes;
		bytes = sprintf(content + pos, "%d & %d \\\\ ", E, F); pos += bytes;
		bytes = sprintf(content + pos, "%d & %d \\\\ ", G, H); pos += bytes;
		bytes = sprintf(content + pos, "\\end{bmatrix} \\)</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p>\\( C \\ = \\ \\begin{bmatrix} "); pos += bytes;
		bytes = sprintf(content + pos, "%d & %d \\\\ ", I, J); pos += bytes;
		bytes = sprintf(content + pos, "%d & %d \\\\ ", K, L); pos += bytes;
		bytes = sprintf(content + pos, "\\end{bmatrix} \\)</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p>Type a matrix.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "{\\begin{bmatrix}{`%d`}&{`%d`}\\\\{`%d`}&{`%d`}\\end{bmatrix}}", 
			I*(A*E+B*G)+K*(A*F+B*H), J*(A*E+B*G)+L*(A*F+B*H),
			I*(C*E+D*G)+K*(C*F+D*H), J*(C*E+D*G)+L*(C*F+D*H)); pos += bytes;	

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_5_28(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 39 - 19;
		}
		while (A == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>A greyscale image is provided.</p>"); pos += bytes;
		
		if (randomizer% 5 == 0)
		{
			bytes = sprintf(content + pos, "<p><table border='1'><tr>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:lightgrey;'>&nbsp;&nbsp;&nbsp;</td>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:darkgrey;'>&nbsp;&nbsp;&nbsp;</td>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:lightgrey;'>&nbsp;&nbsp;&nbsp;</td></tr><tr>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:darkgrey;'>&nbsp;&nbsp;&nbsp;</td>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:darkgrey;'>&nbsp;&nbsp;&nbsp;</td>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:darkgrey;'>&nbsp;&nbsp;&nbsp;</td></tr><tr>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:lightgrey;'>&nbsp;&nbsp;&nbsp;</td>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:darkgrey;'>&nbsp;&nbsp;&nbsp;</td>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:lightgrey;'>&nbsp;&nbsp;&nbsp;</td></tr></table></p>"); pos += bytes;
		}
		else if (randomizer% 5 == 1)
		{
			bytes = sprintf(content + pos, "<p><table border='1'><tr>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:darkgrey;'>&nbsp;&nbsp;&nbsp;</td>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:darkgrey;'>&nbsp;&nbsp;&nbsp;</td>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:darkgrey;'>&nbsp;&nbsp;&nbsp;</td></tr><tr>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:darkgrey;'>&nbsp;&nbsp;&nbsp;</td>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:lightgrey;'>&nbsp;&nbsp;&nbsp;</td>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:darkgrey;'>&nbsp;&nbsp;&nbsp;</td></tr><tr>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:darkgrey;'>&nbsp;&nbsp;&nbsp;</td>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:darkgrey;'>&nbsp;&nbsp;&nbsp;</td>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:darkgrey;'>&nbsp;&nbsp;&nbsp;</td></tr></table></p>"); pos += bytes;
		}
		else if (randomizer% 5 == 2)
		{
			bytes = sprintf(content + pos, "<p><table border='1'><tr>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:darkgrey;'>&nbsp;&nbsp;&nbsp;</td>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:lightgrey;'>&nbsp;&nbsp;&nbsp;</td>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:darkgrey;'>&nbsp;&nbsp;&nbsp;</td></tr><tr>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:lightgrey;'>&nbsp;&nbsp;&nbsp;</td>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:darkgrey;'>&nbsp;&nbsp;&nbsp;</td>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:lightgrey;'>&nbsp;&nbsp;&nbsp;</td></tr><tr>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:darkgrey;'>&nbsp;&nbsp;&nbsp;</td>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:lightgrey;'>&nbsp;&nbsp;&nbsp;</td>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:darkgrey;'>&nbsp;&nbsp;&nbsp;</td></tr></table></p>"); pos += bytes;
		}
		else if (randomizer% 5 == 3)
		{
			bytes = sprintf(content + pos, "<p><table border='1'><tr>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:darkgrey;'>&nbsp;&nbsp;&nbsp;</td>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:lightgrey;'>&nbsp;&nbsp;&nbsp;</td>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:darkgrey;'>&nbsp;&nbsp;&nbsp;</td></tr><tr>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:darkgrey;'>&nbsp;&nbsp;&nbsp;</td>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:lightgrey;'>&nbsp;&nbsp;&nbsp;</td>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:darkgrey;'>&nbsp;&nbsp;&nbsp;</td></tr><tr>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:darkgrey;'>&nbsp;&nbsp;&nbsp;</td>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:darkgrey;'>&nbsp;&nbsp;&nbsp;</td>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:darkgrey;'>&nbsp;&nbsp;&nbsp;</td></tr></table></p>"); pos += bytes;
		}
		else if (randomizer% 5 == 4)
		{
			bytes = sprintf(content + pos, "<p><table border='1'><tr>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:darkgrey;'>&nbsp;&nbsp;&nbsp;</td>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:lightgrey;'>&nbsp;&nbsp;&nbsp;</td>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:darkgrey;'>&nbsp;&nbsp;&nbsp;</td></tr><tr>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:darkgrey;'>&nbsp;&nbsp;&nbsp;</td>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:lightgrey;'>&nbsp;&nbsp;&nbsp;</td>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:darkgrey;'>&nbsp;&nbsp;&nbsp;</td></tr><tr>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:lightgrey;'>&nbsp;&nbsp;&nbsp;</td>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:darkgrey;'>&nbsp;&nbsp;&nbsp;</td>"); pos += bytes;
			bytes = sprintf(content + pos, "<td style='background-color:lightgrey;'>&nbsp;&nbsp;&nbsp;</td></tr></table></p>"); pos += bytes;
		}

		bytes = sprintf(content + pos, "<p>The values of each pixel are assigned by this table.</p>"); pos += bytes;
		bytes = sprintf(content + pos, "<p><table border='1'>"); pos += bytes;
		bytes = sprintf(content + pos, "<tr><td style='background-color:white;'>&nbsp;&nbsp;&nbsp;</td><td>White = 0</td></tr>"); pos += bytes;
		bytes = sprintf(content + pos, "<tr><td style='background-color:lightgrey;'>&nbsp;&nbsp;&nbsp;</td><td>Light Grey = 1</td></tr>"); pos += bytes;
		bytes = sprintf(content + pos, "<tr><td style='background-color:darkgrey;'>&nbsp;&nbsp;&nbsp;</td><td>Dark Grey = 2</td></tr>"); pos += bytes;
		bytes = sprintf(content + pos, "<tr><td style='background-color:black;'>&nbsp;&nbsp;&nbsp;</td><td>Black = 3</td></tr></table></p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p>Use the greyscale image as a matrix.  In order to increase the contrast, any light grey value will turn to white, "); pos += bytes;
		bytes = sprintf(content + pos, "and any dark grey value will turn to black.</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p>What is the matrix representing the image after the contrast has been increased?</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p>Type a matrix.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		if (randomizer% 5 == 0)
		{
			bytes = sprintf(answers + pos,  "{\\begin{bmatrix}{`%d`}&{`%d`}&{`%d`}\\\\{`%d`}&{`%d`}&{`%d`}\\\\{`%d`}&{`%d`}&{`%d`}\\end{bmatrix}}", 
				0, 3, 0,
				3, 3, 3,
				0, 3, 0); pos += bytes;	
		}
		else if (randomizer% 5 == 1)
		{
			bytes = sprintf(answers + pos,  "{\\begin{bmatrix}{`%d`}&{`%d`}&{`%d`}\\\\{`%d`}&{`%d`}&{`%d`}\\\\{`%d`}&{`%d`}&{`%d`}\\end{bmatrix}}", 
				3, 3, 3,
				3, 0, 3,
				3, 3, 3); pos += bytes;	
		}
		else if (randomizer% 5 == 2)
		{
			bytes = sprintf(answers + pos,  "{\\begin{bmatrix}{`%d`}&{`%d`}&{`%d`}\\\\{`%d`}&{`%d`}&{`%d`}\\\\{`%d`}&{`%d`}&{`%d`}\\end{bmatrix}}", 
				3, 0, 3,
				0, 3, 0,
				3, 0, 3); pos += bytes;	
		}	
		else if (randomizer% 5 == 3)
		{
			bytes = sprintf(answers + pos,  "{\\begin{bmatrix}{`%d`}&{`%d`}&{`%d`}\\\\{`%d`}&{`%d`}&{`%d`}\\\\{`%d`}&{`%d`}&{`%d`}\\end{bmatrix}}", 
				3, 0, 3,
				3, 0, 3,
				3, 3, 3); pos += bytes;	
		}
		else if (randomizer% 5 == 4)
		{
			bytes = sprintf(answers + pos,  "{\\begin{bmatrix}{`%d`}&{`%d`}&{`%d`}\\\\{`%d`}&{`%d`}&{`%d`}\\\\{`%d`}&{`%d`}&{`%d`}\\end{bmatrix}}", 
				3, 0, 3,
				3, 0, 3,
				0, 3, 0); pos += bytes;	
		}

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_5_29(const char *f)
{
	// ???

	return 1;
};

int question_5_30(const char *f)
{
	// ???

	return 1;
};


int question_6_01(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 4 + 2;
			B = rand() % 4 + 2;
			C = rand() % 4 + 2;
		}
		while (A == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Use the Fundamental Counting Principle to solve.</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p>The model of the car you are thinking of buying is available in "); pos += bytes;

		if (A == 2) { bytes = sprintf(content + pos, "two"); pos += bytes; }
		else if (A == 3) { bytes = sprintf(content + pos, "three"); pos += bytes; }
		else if (A == 4) { bytes = sprintf(content + pos, "four"); pos += bytes; }
		else if (A == 5) { bytes = sprintf(content + pos, "five"); pos += bytes; }

		bytes = sprintf(content + pos, " different paint colors and "); pos += bytes;
		
		if (B == 2) { bytes = sprintf(content + pos, "two"); pos += bytes; }
		else if (B == 3) { bytes = sprintf(content + pos, "three"); pos += bytes; }
		else if (B == 4) { bytes = sprintf(content + pos, "four"); pos += bytes; }
		else if (B == 5) { bytes = sprintf(content + pos, "five"); pos += bytes; }

		bytes = sprintf(content + pos, " different interiors and "); pos += bytes;
		
		if (C == 2) { bytes = sprintf(content + pos, "two"); pos += bytes; } 
		else if (C == 3) { bytes = sprintf(content + pos, "three"); pos += bytes; }
		else if (C == 4) { bytes = sprintf(content + pos, "four"); pos += bytes; }
		else if (C == 5) { bytes = sprintf(content + pos, "five"); pos += bytes; }

		bytes = sprintf(content + pos, " different wheels. In how many ways can you order the​ car?</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%d", A*B*C); pos += bytes;	

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_6_02(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 8 + 2;
			B = rand() % 8 + 2;
			C = rand() % 8 + 2;
		}
		while (A == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Use the Fundamental Counting Principle to solve.</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p>An ice cream store sells "); pos += bytes;
		bytes = sprintf(content + pos, "%d drinks, in ", A); pos += bytes;
		bytes = sprintf(content + pos, "%d sizes, and ", B); pos += bytes;
		bytes = sprintf(content + pos, "%d flavors. In how many ways can a customer order a​ drink?</p>", C); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos,  "%d", A*B*C); pos += bytes;	

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_6_03(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 4 + 6;
			B = rand() % 4 + 2;
		}
		while (A == 0 || A <= B || factorial(A) / factorial(A-B) > 1000 || factorial(A) / (factorial(A-B) * factorial(B)) > 1000);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>Use the formula for \\( _{n}P_{r} \\) or \\( _{n}C_{r} \\) to solve.</p>"); pos += bytes;

		if (randomizer % 10 <= 4)
		{
			bytes = sprintf(content + pos, "<p>There are %d automobiles in a race with no ties, but only the first %d receive a medal.</p>", A, B); pos += bytes;
			bytes = sprintf(content + pos, "<p>How many ways can the first %d finishers come in?</p>", B); pos += bytes;
		}
		else
		{
			bytes = sprintf(content + pos, "<p>Of %d possible books, you plan to take %d with you on vacation.</p>", A, B); pos += bytes;
			bytes = sprintf(content + pos, "<p>How many different collections of %d books can you​ take?</p>", B); pos += bytes;	
		}

		bytes = 0;
		pos = 0;			

		if (randomizer % 10 <= 4)
		{
			bytes = sprintf(answers + pos,  "%d", 
				factorial(A) / factorial(A-B)); pos += bytes;
		}
		else
		{
			bytes = sprintf(answers + pos,  "%d", 
				factorial(A) / (factorial(A-B) * factorial(B))); pos += bytes;
		}

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_6_04(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	float r;

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			for (int i=0; i<10; i++) r = (float)(rand() % 100); // just to randomize more???

			A = rand() % 199 + 600;
			B = rand() % 199 + 600;
			C = rand() % 399 + 1000;
			D = rand() % 399 + 1000;
			E = rand() % 299 + 800;
			F = rand() % 299 + 800;

			G = rand() % 3;
			H = rand() % 2;

			if (G == 0)
			{
				if (H == 0) r = (float)A/(float)(A+D);
				else if (H == 1) r = (float)D/(float)(A+D);
			}
			else if (G == 1)	
			{
				if (H == 0) r = (float)B/(float)(B+E);
				else if (H == 1) r = (float)E/(float)(B+E);
			}
			else if (G == 2)	
			{
				if (H == 0) r = (float)C/(float)(C+F);
				else if (H == 1) r = (float)F/(float)(C+F);
			}
		}
		while (A == 0 || (int)(r * 1000.0f) % 10 >= 5 || (int)(r * 100.0f) % 10 < 1);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>The table shows the distribution of favorite fast foods.</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p><table border='1'><tr><td></td><td>Pizzas</td><td>Burgers</td><td>Tacos</td><td>Total</td></tr>"); pos += bytes;
		bytes = sprintf(content + pos, "<tr><td>Male</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td></tr>", A, B, C, A + B + C); pos += bytes;
		bytes = sprintf(content + pos, "<tr><td>Female</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td></tr>", D, E, F, D + E + F); pos += bytes;
		bytes = sprintf(content + pos, "<tr><td>Total</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td></tr></table>", A+D, B+E, C+F, A+B+C+D+E+F); pos += bytes;		

		bytes = sprintf(content + pos, "<p>If one person is randomly selected from among %s fans, find the probabilty of selecting a %s.</p>",
			(G==0?"pizza":(G==1?"burger":"taco")), (H==0?"male":"female")); pos += bytes;

		bytes = sprintf(content + pos, "<p>Round to 2 decimal places.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos, "%.2f\t", r); pos += bytes;
		bytes = sprintf(answers + pos, ".%d", (int)(r * 100)); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_6_05(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 199 + 600;
			B = rand() % 199 + 600;
			C = rand() % 399 + 1000;
			D = rand() % 399 + 1000;
			E = rand() % 299 + 800;
			F = rand() % 299 + 800;

			G = rand() % 3;
			H = rand() % 2;

			if (G == 0)
			{
				if (H == 0) I = A + B + C + D;
				else if (H == 1) I = A + D + E + F;
			}
			else if (G == 1)	
			{
				if (H == 0) I = A + B + C + E;
				else if (H == 1) I = B + D + E + F;
			}
			else if (G == 2)	
			{
				if (H == 0) I = A + B + C + F;
				else if (H == 1) I = C + D + E + F;
			}

			J = A + B + C + D + E + F;
		}
		while (A == 0 || !isprime(I) || !isprime(J) || I == J);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>The table shows the distribution of favorite preferred drinks.</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p><table border='1'><tr><td></td><td>Soda</td><td>Coffee</td><td>Tea</td><td>Total</td></tr>"); pos += bytes;
		bytes = sprintf(content + pos, "<tr><td>Male</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td></tr>", A, B, C, A + B + C); pos += bytes;
		bytes = sprintf(content + pos, "<tr><td>Female</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td></tr>", D, E, F, D + E + F); pos += bytes;
		bytes = sprintf(content + pos, "<tr><td>Total</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td></tr></table>", A+D, B+E, C+F, A+B+C+D+E+F); pos += bytes;		

		bytes = sprintf(content + pos, "<p>What is the probability of selecting a %s fan or a %s.</p>",
			(G==0?"soda":(G==1?"coffee":"tea")), (H==0?"male":"female")); pos += bytes;

		bytes = sprintf(content + pos, "<p>Type a simplified fraction.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		bytes = sprintf(answers + pos, "{\\dfrac{`%d`}{`%d`}}", I, J); pos += bytes;

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;

	return 1;
};

int question_6_06(const char *f)
{
	if (strcmp(f, "single") != 0)
	{
		if (!fcreate(f))
		{
			return 0; 
		}
	}

	for (int i=0; i<pool_size; i++)
	{
		do
		{
			A = rand() % 9 + 2;

			B = rand() % 6;

			if (B <= 3)
			{
				C = 4;
				D = 13;
			}
			else
			{
				C = 7;
				D = 13;
			} 
		}
		while (A == 0);

		for (int j=0; j<4096; j++) { content[j] = 0; answers[j] = 0; }
	
		bytes = 0;
		pos = 0;

		bytes = sprintf(content + pos, "<p>You randomly select a card from a​ 52-card deck.</p>"); pos += bytes;

		bytes = sprintf(content + pos, "<p>Find the probability of selecting a %d %s a %s card.</p>",
			A, (randomizer%2==0?"and":"or"), (B==0?"spade":(B==1?"club":(B==2?"heart":(B==3?"diamond":(B==4?"black":"red")))))); pos += bytes;		

		bytes = sprintf(content + pos, "<p>Type a simplified fraction.</p>"); pos += bytes;

		bytes = 0;
		pos = 0;			

		if (randomizer% 2 == 0)
		{
			if (B <= 3) { bytes = sprintf(answers + pos, "{\\dfrac{`%d`}{`%d`}}", 1, 52); pos += bytes; }
			else { bytes = sprintf(answers + pos, "{\\dfrac{`%d`}{`%d`}}", 1, 26); pos += bytes; }
		}
		else if (randomizer% 2 == 1)
		{
			bytes = sprintf(answers + pos, "{\\dfrac{`%d`}{`%d`}}", C, D); pos += bytes;
		}

		format(answers, answers);

		if (!compile(content, answers))
		{
			return 0;
		}
	}
		
	if (strcmp(f, "single") != 0)
	{
		fclose(output);
	} randomizer++;


	return 1;
};



int main(const int argc, const char **argv)
{
	for (unsigned long i=0; i<time(0)%1000; i++) randomizer = rand() % 100;

	for (int i=0; i<256; i++) file[i] = 0;

	sprintf(file, "%s", argv[0]);

	if (argc == 1)
	{
		printf("Blackboard Test Generator 2\n");
		printf("Arguments:\n");
		printf("pools <size> - Creates All Question Pools from Code\n");
		printf("  All pooled questions will be put into a folder called 'Pools/'.\n");
		printf("single - Creates a Single Question from each Code\n");
		printf("  The one pool will be put in 'Single.txt'.\n");
		printf("graph <code> - Creates a Graph using 'gnuplot'\n");
		printf("  This is primarily for internal use by the program.\n");
		printf("custom <filename> <content> <answers> - Creates a Custom Question\n");
		printf("  Uses @...@ in content to indicates 'gnuplot' code inline.\n");
		printf("  The function to be printed is 'f(x)'.\n");
		printf("  Answers are separated with tabs, using the \\t character.\n");
		printf("  Answers are partially formatted, characters inside { } are unchanged, ");
		printf("    while characters inside ` ` are always changed.\n");
		printf("  Example:\n");
		printf("  %s custom SingleQuestion.txt \"<p>Type: \\\\((x-3)^{2}\\\\)</p><p>@f(x)=(x-3)**2@</p>\" \"(x-3){^{`2`}}\"\n", argv[0]);
		
		return 1;
	}
	else if (strcmp(argv[1], "graph") == 0)
	{
		Gnuplot gp;

		gp << argv[2];

		return 1;
	}
	else if (strcmp(argv[1], "custom") == 0)
	{
		printf("%s\n", argv[3]);

		if (!fcreate(argv[2]))
		{
			printf("Single Failure to Create\n");

			return 0;
		}

		if (!compile(argv[3], argv[4]))
		{
			printf("Single Failure to Compile\n");

			return 0;
		}
	
		fclose(output);

		printf("Custom Complete!\n");

		return 1;
	}
	else if (strcmp(argv[1], "single") == 0)
	{
		int r;

		for (unsigned long t=0; t<time(0) % 10000; t++) { r = rand() % 100; }

		for (int i=0; i<time(0)%1000; i++)
		{
			if (rand() % 100 < 0) { }
		}

		pool_size = 1;

		attempts = 5;

		if (!fcreate("Single-1.txt"))
		{
			return 0;
		}

		printf("Lesson 1\n");

		if (!question_1_01("single")) return 0;
		if (!question_1_02("single")) return 0;
		if (!question_1_03("single")) return 0;
		if (!question_1_04("single")) return 0;
		if (!question_1_05("single")) return 0;
		if (!question_1_06("single")) return 0;
		if (!question_1_07("single")) return 0;
		if (!question_1_08("single")) return 0;
		if (!question_1_09("single")) return 0;
		if (!question_1_10("single")) return 0;
		if (!question_1_11("single")) return 0;
		if (!question_1_12("single")) return 0;
		if (!question_1_13("single")) return 0;
		if (!question_1_14("single")) return 0;
		if (!question_1_15("single")) return 0;
		if (!question_1_16("single")) return 0;
		if (!question_1_17("single")) return 0;
		if (!question_1_18("single")) return 0;
		if (!question_1_19("single")) return 0;
		if (!question_1_20("single")) return 0;
		if (!question_1_21("single")) return 0;
		if (!question_1_22("single")) return 0;
		if (!question_1_23("single")) return 0;
		if (!question_1_24("single")) return 0;
		if (!question_1_25("single")) return 0;
		if (!question_1_26("single")) return 0;
		if (!question_1_27("single")) return 0;
		if (!question_1_28("single")) return 0;
		if (!question_1_29("single")) return 0;
		if (!question_1_30("single")) return 0;

		fclose(output);

		if (!fcreate("Single-2.txt"))
		{
			return 0;
		}

		printf("Lesson 2\n");

		if (!question_2_01("single")) return 0;
		if (!question_2_02("single")) return 0;
		if (!question_2_03("single")) return 0;
		if (!question_2_04("single")) return 0;
		if (!question_2_05("single")) return 0;
		if (!question_2_06("single")) return 0;
		if (!question_2_07("single")) return 0;
		if (!question_2_08("single")) return 0;
		if (!question_2_09("single")) return 0;
		if (!question_2_10("single")) return 0;
		if (!question_2_11("single")) return 0;
		if (!question_2_12("single")) return 0;
		if (!question_2_13("single")) return 0;
		if (!question_2_14("single")) return 0;
		if (!question_2_15("single")) return 0;
		if (!question_2_16("single")) return 0;
		if (!question_2_17("single")) return 0;
		if (!question_2_18("single")) return 0;
		if (!question_2_19("single")) return 0;
		if (!question_2_20("single")) return 0;
		if (!question_2_21("single")) return 0;
		if (!question_2_22("single")) return 0;
		if (!question_2_23("single")) return 0;
		if (!question_2_24("single")) return 0;
		if (!question_2_25("single")) return 0;
		if (!question_2_26("single")) return 0;
		if (!question_2_27("single")) return 0;
		if (!question_2_28("single")) return 0;
		if (!question_2_29("single")) return 0;
		if (!question_2_30("single")) return 0;

		fclose(output);

		if (!fcreate("Single-3.txt"))
		{
			return 0;
		}

		printf("Lesson 3\n");

		if (!question_3_01("single")) return 0;
		if (!question_3_02("single")) return 0;
		if (!question_3_03("single")) return 0;
		if (!question_3_04("single")) return 0;
		if (!question_3_05("single")) return 0;
		if (!question_3_06("single")) return 0;
		if (!question_3_07("single")) return 0;
		if (!question_3_08("single")) return 0;
		if (!question_3_09("single")) return 0;
		if (!question_3_10("single")) return 0;
		if (!question_3_11("single")) return 0;
		if (!question_3_12("single")) return 0;
		if (!question_3_13("single")) return 0;
		if (!question_3_14("single")) return 0;
		if (!question_3_15("single")) return 0;
		if (!question_3_16("single")) return 0;
		if (!question_3_17("single")) return 0;
		if (!question_3_18("single")) return 0;
		if (!question_3_19("single")) return 0;
		if (!question_3_20("single")) return 0;
		if (!question_3_21("single")) return 0;
		if (!question_3_22("single")) return 0;
		if (!question_3_23("single")) return 0;
		if (!question_3_24("single")) return 0;
		if (!question_3_25("single")) return 0;
		if (!question_3_26("single")) return 0;
		if (!question_3_27("single")) return 0;
		if (!question_3_28("single")) return 0;
		if (!question_3_29("single")) return 0;
		if (!question_3_30("single")) return 0;

		fclose(output);

		if (!fcreate("Single-4.txt"))
		{
			return 0;
		}

		printf("Lesson 4\n");

		if (!question_4_01("single")) return 0;
		if (!question_4_02("single")) return 0;
		if (!question_4_03("single")) return 0;
		if (!question_4_04("single")) return 0;
		if (!question_4_05("single")) return 0;
		if (!question_4_06("single")) return 0;
		if (!question_4_07("single")) return 0;
		if (!question_4_08("single")) return 0;
		if (!question_4_09("single")) return 0;
		if (!question_4_10("single")) return 0;
		if (!question_4_11("single")) return 0;
		if (!question_4_12("single")) return 0;
		if (!question_4_13("single")) return 0;
		if (!question_4_14("single")) return 0;
		if (!question_4_15("single")) return 0;
		if (!question_4_16("single")) return 0;
		if (!question_4_17("single")) return 0;
		if (!question_4_18("single")) return 0;
		if (!question_4_19("single")) return 0;
		if (!question_4_20("single")) return 0;
		if (!question_4_21("single")) return 0;
		if (!question_4_22("single")) return 0;
		if (!question_4_23("single")) return 0;
		if (!question_4_24("single")) return 0;
		if (!question_4_25("single")) return 0;
		if (!question_4_26("single")) return 0;
		if (!question_4_27("single")) return 0;
		if (!question_4_28("single")) return 0;
		if (!question_4_29("single")) return 0;
		if (!question_4_30("single")) return 0;

		fclose(output);

		if (!fcreate("Single-5.txt"))
		{
			return 0;
		}

		printf("Lesson 5\n");

		if (!question_5_01("single")) return 0;
		if (!question_5_02("single")) return 0;
		if (!question_5_03("single")) return 0;
		if (!question_5_04("single")) return 0;
		if (!question_5_05("single")) return 0;
		if (!question_5_06("single")) return 0;
		if (!question_5_07("single")) return 0;
		if (!question_5_08("single")) return 0;
		if (!question_5_09("single")) return 0;
		if (!question_5_10("single")) return 0;
		if (!question_5_11("single")) return 0;
		if (!question_5_12("single")) return 0;
		if (!question_5_13("single")) return 0;
		if (!question_5_14("single")) return 0;
		if (!question_5_15("single")) return 0;
		if (!question_5_16("single")) return 0;
		if (!question_5_17("single")) return 0;
		if (!question_5_18("single")) return 0;
		if (!question_5_19("single")) return 0;
		if (!question_5_20("single")) return 0;
		if (!question_5_21("single")) return 0;
		if (!question_5_22("single")) return 0;
		if (!question_5_23("single")) return 0;
		if (!question_5_24("single")) return 0;
		if (!question_5_25("single")) return 0;
		if (!question_5_26("single")) return 0;
		if (!question_5_27("single")) return 0;
		if (!question_5_28("single")) return 0;
		if (!question_5_29("single")) return 0;
		if (!question_5_30("single")) return 0;

		fclose(output);

		if (!fcreate("Single-6.txt"))
		{
			return 0;
		}

		printf("Lesson 6\n");

		if (!question_6_01("single")) return 0;
		if (!question_6_02("single")) return 0;
		if (!question_6_03("single")) return 0;
		if (!question_6_04("single")) return 0;
		if (!question_6_05("single")) return 0;
		if (!question_6_06("single")) return 0;
		
		fclose(output);

		printf("Single Complete!\n");
	}
	else if (strcmp(argv[1], "pools") == 0)
	{
		int r;

		for (unsigned long t=0; t<time(0) % 10000; t++) { r = rand() % 100; }

		system("mkdir Pools"); // for Linux only???

		for (int i=0; i<time(0)%1000; i++)
		{
			if (rand() % 100 < 0) { }
		}

		pool_size = atoi(argv[2]);

		attempts = 5;

		if (!question_1_01("Pools/Practice-1-01.txt")) return 0;
		if (!question_1_02("Pools/Practice-1-02.txt")) return 0;
		if (!question_1_03("Pools/Practice-1-03.txt")) return 0;
		if (!question_1_04("Pools/Practice-1-04.txt")) return 0;
		if (!question_1_05("Pools/Practice-1-05.txt")) return 0;
		if (!question_1_06("Pools/Practice-1-06.txt")) return 0;
		if (!question_1_07("Pools/Practice-1-07.txt")) return 0;
		if (!question_1_08("Pools/Practice-1-08.txt")) return 0;
		if (!question_1_09("Pools/Practice-1-09.txt")) return 0;
		if (!question_1_10("Pools/Practice-1-10.txt")) return 0;
		if (!question_1_11("Pools/Practice-1-11.txt")) return 0;
		if (!question_1_12("Pools/Practice-1-12.txt")) return 0;
		if (!question_1_13("Pools/Practice-1-13.txt")) return 0;
		if (!question_1_14("Pools/Practice-1-14.txt")) return 0;
		if (!question_1_15("Pools/Practice-1-15.txt")) return 0;
		if (!question_1_16("Pools/Practice-1-16.txt")) return 0;
		if (!question_1_17("Pools/Practice-1-17.txt")) return 0;
		if (!question_1_18("Pools/Practice-1-18.txt")) return 0;
		if (!question_1_19("Pools/Practice-1-19.txt")) return 0;
		if (!question_1_20("Pools/Practice-1-20.txt")) return 0;
		if (!question_1_21("Pools/Practice-1-21.txt")) return 0;
		if (!question_1_22("Pools/Practice-1-22.txt")) return 0;
		if (!question_1_23("Pools/Practice-1-23.txt")) return 0;
		if (!question_1_24("Pools/Practice-1-24.txt")) return 0;
		if (!question_1_25("Pools/Practice-1-25.txt")) return 0;
		if (!question_1_26("Pools/Practice-1-26.txt")) return 0;
		if (!question_1_27("Pools/Practice-1-27.txt")) return 0;
		if (!question_1_28("Pools/Practice-1-28.txt")) return 0;
		if (!question_1_29("Pools/Practice-1-29.txt")) return 0;
		if (!question_1_30("Pools/Practice-1-30.txt")) return 0;

		if (!question_2_01("Pools/Practice-2-01.txt")) return 0;
		if (!question_2_02("Pools/Practice-2-02.txt")) return 0;
		if (!question_2_03("Pools/Practice-2-03.txt")) return 0;
		if (!question_2_04("Pools/Practice-2-04.txt")) return 0;
		if (!question_2_05("Pools/Practice-2-05.txt")) return 0;
		if (!question_2_06("Pools/Practice-2-06.txt")) return 0;
		if (!question_2_07("Pools/Practice-2-07.txt")) return 0;
		if (!question_2_08("Pools/Practice-2-08.txt")) return 0;
		if (!question_2_09("Pools/Practice-2-09.txt")) return 0;
		if (!question_2_10("Pools/Practice-2-10.txt")) return 0;
		if (!question_2_11("Pools/Practice-2-11.txt")) return 0;
		if (!question_2_12("Pools/Practice-2-12.txt")) return 0;
		if (!question_2_13("Pools/Practice-2-13.txt")) return 0;
		if (!question_2_14("Pools/Practice-2-14.txt")) return 0;
		if (!question_2_15("Pools/Practice-2-15.txt")) return 0;
		if (!question_2_16("Pools/Practice-2-16.txt")) return 0;
		if (!question_2_17("Pools/Practice-2-17.txt")) return 0;
		if (!question_2_18("Pools/Practice-2-18.txt")) return 0;
		if (!question_2_19("Pools/Practice-2-19.txt")) return 0;
		if (!question_2_20("Pools/Practice-2-20.txt")) return 0;
		if (!question_2_21("Pools/Practice-2-21.txt")) return 0;
		if (!question_2_22("Pools/Practice-2-22.txt")) return 0;
		if (!question_2_23("Pools/Practice-2-23.txt")) return 0;
		if (!question_2_24("Pools/Practice-2-24.txt")) return 0;
		if (!question_2_25("Pools/Practice-2-25.txt")) return 0;
		if (!question_2_26("Pools/Practice-2-26.txt")) return 0;
		if (!question_2_27("Pools/Practice-2-27.txt")) return 0;
		if (!question_2_28("Pools/Practice-2-28.txt")) return 0;
		if (!question_2_29("Pools/Practice-2-29.txt")) return 0;
		if (!question_2_30("Pools/Practice-2-30.txt")) return 0;

		if (!question_3_01("Pools/Practice-3-01.txt")) return 0;
		if (!question_3_02("Pools/Practice-3-02.txt")) return 0;
		if (!question_3_03("Pools/Practice-3-03.txt")) return 0;
		if (!question_3_04("Pools/Practice-3-04.txt")) return 0;
		if (!question_3_05("Pools/Practice-3-05.txt")) return 0;
		if (!question_3_06("Pools/Practice-3-06.txt")) return 0;
		if (!question_3_07("Pools/Practice-3-07.txt")) return 0;
		if (!question_3_08("Pools/Practice-3-08.txt")) return 0;
		if (!question_3_09("Pools/Practice-3-09.txt")) return 0;
		if (!question_3_10("Pools/Practice-3-10.txt")) return 0;
		if (!question_3_11("Pools/Practice-3-11.txt")) return 0;
		if (!question_3_12("Pools/Practice-3-12.txt")) return 0;
		if (!question_3_13("Pools/Practice-3-13.txt")) return 0;
		if (!question_3_14("Pools/Practice-3-14.txt")) return 0;
		if (!question_3_15("Pools/Practice-3-15.txt")) return 0;
		if (!question_3_16("Pools/Practice-3-16.txt")) return 0;
		if (!question_3_17("Pools/Practice-3-17.txt")) return 0;
		if (!question_3_18("Pools/Practice-3-18.txt")) return 0;
		if (!question_3_19("Pools/Practice-3-19.txt")) return 0;
		if (!question_3_20("Pools/Practice-3-20.txt")) return 0;
		if (!question_3_21("Pools/Practice-3-21.txt")) return 0;
		if (!question_3_22("Pools/Practice-3-22.txt")) return 0;
		if (!question_3_23("Pools/Practice-3-23.txt")) return 0;
		if (!question_3_24("Pools/Practice-3-24.txt")) return 0;
		if (!question_3_25("Pools/Practice-3-25.txt")) return 0;
		if (!question_3_26("Pools/Practice-3-26.txt")) return 0;
		if (!question_3_27("Pools/Practice-3-27.txt")) return 0;
		if (!question_3_28("Pools/Practice-3-28.txt")) return 0;
		if (!question_3_29("Pools/Practice-3-29.txt")) return 0;
		if (!question_3_30("Pools/Practice-3-30.txt")) return 0;

		if (!question_4_01("Pools/Practice-4-01.txt")) return 0;
		if (!question_4_02("Pools/Practice-4-02.txt")) return 0;
		if (!question_4_03("Pools/Practice-4-03.txt")) return 0;
		if (!question_4_04("Pools/Practice-4-04.txt")) return 0;
		if (!question_4_05("Pools/Practice-4-05.txt")) return 0;
		if (!question_4_06("Pools/Practice-4-06.txt")) return 0;
		if (!question_4_07("Pools/Practice-4-07.txt")) return 0;
		if (!question_4_08("Pools/Practice-4-08.txt")) return 0;
		if (!question_4_09("Pools/Practice-4-09.txt")) return 0;
		if (!question_4_10("Pools/Practice-4-10.txt")) return 0;
		if (!question_4_11("Pools/Practice-4-11.txt")) return 0;
		if (!question_4_12("Pools/Practice-4-12.txt")) return 0;
		if (!question_4_13("Pools/Practice-4-13.txt")) return 0;
		if (!question_4_14("Pools/Practice-4-14.txt")) return 0;
		if (!question_4_15("Pools/Practice-4-15.txt")) return 0;
		if (!question_4_16("Pools/Practice-4-16.txt")) return 0;
		if (!question_4_17("Pools/Practice-4-17.txt")) return 0;
		if (!question_4_18("Pools/Practice-4-18.txt")) return 0;
		if (!question_4_19("Pools/Practice-4-19.txt")) return 0;
		if (!question_4_20("Pools/Practice-4-20.txt")) return 0;
		if (!question_4_21("Pools/Practice-4-21.txt")) return 0;
		if (!question_4_22("Pools/Practice-4-22.txt")) return 0;
		if (!question_4_23("Pools/Practice-4-23.txt")) return 0;
		if (!question_4_24("Pools/Practice-4-24.txt")) return 0;
		if (!question_4_25("Pools/Practice-4-25.txt")) return 0;
		if (!question_4_26("Pools/Practice-4-26.txt")) return 0;
		if (!question_4_27("Pools/Practice-4-27.txt")) return 0;
		if (!question_4_28("Pools/Practice-4-28.txt")) return 0;
		if (!question_4_29("Pools/Practice-4-29.txt")) return 0;
		if (!question_4_30("Pools/Practice-4-30.txt")) return 0;

		if (!question_5_01("Pools/Practice-5-01.txt")) return 0;
		if (!question_5_02("Pools/Practice-5-02.txt")) return 0;
		if (!question_5_03("Pools/Practice-5-03.txt")) return 0;
		if (!question_5_04("Pools/Practice-5-04.txt")) return 0;
		if (!question_5_05("Pools/Practice-5-05.txt")) return 0;
		if (!question_5_06("Pools/Practice-5-06.txt")) return 0;
		if (!question_5_07("Pools/Practice-5-07.txt")) return 0;
		if (!question_5_08("Pools/Practice-5-08.txt")) return 0;
		if (!question_5_09("Pools/Practice-5-09.txt")) return 0;
		if (!question_5_10("Pools/Practice-5-10.txt")) return 0;
		if (!question_5_11("Pools/Practice-5-11.txt")) return 0;
		if (!question_5_12("Pools/Practice-5-12.txt")) return 0;
		if (!question_5_13("Pools/Practice-5-13.txt")) return 0;
		if (!question_5_14("Pools/Practice-5-14.txt")) return 0;
		if (!question_5_15("Pools/Practice-5-15.txt")) return 0;
		if (!question_5_16("Pools/Practice-5-16.txt")) return 0;
		if (!question_5_17("Pools/Practice-5-17.txt")) return 0;
		if (!question_5_18("Pools/Practice-5-18.txt")) return 0;
		if (!question_5_19("Pools/Practice-5-19.txt")) return 0;
		if (!question_5_20("Pools/Practice-5-20.txt")) return 0;
		if (!question_5_21("Pools/Practice-5-21.txt")) return 0;
		if (!question_5_22("Pools/Practice-5-22.txt")) return 0;
		if (!question_5_23("Pools/Practice-5-23.txt")) return 0;
		if (!question_5_24("Pools/Practice-5-24.txt")) return 0;
		if (!question_5_25("Pools/Practice-5-25.txt")) return 0;
		if (!question_5_26("Pools/Practice-5-26.txt")) return 0;
		if (!question_5_27("Pools/Practice-5-27.txt")) return 0;
		if (!question_5_28("Pools/Practice-5-28.txt")) return 0;
		if (!question_5_29("Pools/Practice-5-29.txt")) return 0;
		if (!question_5_30("Pools/Practice-5-30.txt")) return 0;
	
		if (!question_6_01("Pools/Practice-6-01.txt")) return 0;
		if (!question_6_02("Pools/Practice-6-02.txt")) return 0;
		if (!question_6_03("Pools/Practice-6-03.txt")) return 0;
		if (!question_6_04("Pools/Practice-6-04.txt")) return 0;
		if (!question_6_05("Pools/Practice-6-05.txt")) return 0;
		if (!question_6_06("Pools/Practice-6-06.txt")) return 0;
		
		
		attempts = 0;

		if (!question_1_01("Pools/Temp-1-01.txt")) return 0;
		if (!question_1_02("Pools/Temp-1-02.txt")) return 0;
		system("cat Pools/Temp-1-01.txt > Pools/Quiz-1-01.txt");
		system("cat Pools/Temp-1-02.txt >> Pools/Quiz-1-01.txt");
		if (!question_1_03("Pools/Temp-1-03.txt")) return 0;
		if (!question_1_04("Pools/Temp-1-04.txt")) return 0;
		system("cat Pools/Temp-1-03.txt > Pools/Quiz-1-02.txt");
		system("cat Pools/Temp-1-04.txt >> Pools/Quiz-1-02.txt");
		if (!question_1_05("Pools/Temp-1-05.txt")) return 0;
		if (!question_1_06("Pools/Temp-1-06.txt")) return 0;
		system("cat Pools/Temp-1-05.txt > Pools/Quiz-1-03.txt");
		system("cat Pools/Temp-1-06.txt >> Pools/Quiz-1-03.txt");
		if (!question_1_07("Pools/Temp-1-07.txt")) return 0;
		if (!question_1_08("Pools/Temp-1-08.txt")) return 0;
		system("cat Pools/Temp-1-07.txt > Pools/Quiz-1-04.txt");
		system("cat Pools/Temp-1-08.txt >> Pools/Quiz-1-04.txt");
		if (!question_1_09("Pools/Temp-1-09.txt")) return 0;
		if (!question_1_10("Pools/Temp-1-10.txt")) return 0;
		system("cat Pools/Temp-1-09.txt > Pools/Quiz-1-05.txt");
		system("cat Pools/Temp-1-10.txt >> Pools/Quiz-1-05.txt");
		if (!question_1_11("Pools/Temp-1-11.txt")) return 0;
		if (!question_1_12("Pools/Temp-1-12.txt")) return 0;
		system("cat Pools/Temp-1-11.txt > Pools/Quiz-1-06.txt");
		system("cat Pools/Temp-1-12.txt >> Pools/Quiz-1-06.txt");
		if (!question_1_13("Pools/Temp-1-13.txt")) return 0;
		if (!question_1_14("Pools/Temp-1-14.txt")) return 0;
		system("cat Pools/Temp-1-13.txt > Pools/Quiz-1-07.txt");
		system("cat Pools/Temp-1-14.txt >> Pools/Quiz-1-07.txt");
		if (!question_1_15("Pools/Temp-1-15.txt")) return 0;
		if (!question_1_16("Pools/Temp-1-16.txt")) return 0;
		system("cat Pools/Temp-1-15.txt > Pools/Quiz-1-08.txt");
		system("cat Pools/Temp-1-16.txt >> Pools/Quiz-1-08.txt");
		if (!question_1_17("Pools/Temp-1-17.txt")) return 0;
		if (!question_1_18("Pools/Temp-1-18.txt")) return 0;
		system("cat Pools/Temp-1-17.txt > Pools/Quiz-1-09.txt");
		system("cat Pools/Temp-1-18.txt >> Pools/Quiz-1-09.txt");
		if (!question_1_19("Pools/Temp-1-19.txt")) return 0;
		if (!question_1_20("Pools/Temp-1-20.txt")) return 0;
		system("cat Pools/Temp-1-19.txt > Pools/Quiz-1-10.txt");
		system("cat Pools/Temp-1-20.txt >> Pools/Quiz-1-10.txt");
		if (!question_1_21("Pools/Temp-1-21.txt")) return 0;
		if (!question_1_22("Pools/Temp-1-22.txt")) return 0;
		system("cat Pools/Temp-1-21.txt > Pools/Quiz-1-11.txt");
		system("cat Pools/Temp-1-22.txt >> Pools/Quiz-1-11.txt");
		if (!question_1_23("Pools/Temp-1-23.txt")) return 0;
		if (!question_1_24("Pools/Temp-1-24.txt")) return 0;
		system("cat Pools/Temp-1-23.txt > Pools/Quiz-1-12.txt");
		system("cat Pools/Temp-1-24.txt >> Pools/Quiz-1-12.txt");
		if (!question_1_25("Pools/Temp-1-25.txt")) return 0;
		if (!question_1_26("Pools/Temp-1-26.txt")) return 0;
		system("cat Pools/Temp-1-25.txt > Pools/Quiz-1-13.txt");
		system("cat Pools/Temp-1-26.txt >> Pools/Quiz-1-13.txt");
		if (!question_1_27("Pools/Temp-1-27.txt")) return 0;
		if (!question_1_28("Pools/Temp-1-28.txt")) return 0;
		system("cat Pools/Temp-1-27.txt > Pools/Quiz-1-14.txt");
		system("cat Pools/Temp-1-28.txt >> Pools/Quiz-1-14.txt");
		if (!question_1_29("Pools/Temp-1-29.txt")) return 0;
		if (!question_1_30("Pools/Temp-1-30.txt")) return 0;
		system("cat Pools/Temp-1-29.txt > Pools/Quiz-1-15.txt");
		system("cat Pools/Temp-1-30.txt >> Pools/Quiz-1-15.txt");

		if (!question_2_01("Pools/Temp-2-01.txt")) return 0;
		if (!question_2_02("Pools/Temp-2-02.txt")) return 0;
		system("cat Pools/Temp-2-01.txt > Pools/Quiz-2-01.txt");
		system("cat Pools/Temp-2-02.txt >> Pools/Quiz-2-01.txt");
		if (!question_2_03("Pools/Temp-2-03.txt")) return 0;
		if (!question_2_04("Pools/Temp-2-04.txt")) return 0;
		system("cat Pools/Temp-2-03.txt > Pools/Quiz-2-02.txt");
		system("cat Pools/Temp-2-04.txt >> Pools/Quiz-2-02.txt");
		if (!question_2_05("Pools/Temp-2-05.txt")) return 0;
		if (!question_2_06("Pools/Temp-2-06.txt")) return 0;
		system("cat Pools/Temp-2-05.txt > Pools/Quiz-2-03.txt");
		system("cat Pools/Temp-2-06.txt >> Pools/Quiz-2-03.txt");
		if (!question_2_07("Pools/Temp-2-07.txt")) return 0;
		if (!question_2_08("Pools/Temp-2-08.txt")) return 0;
		system("cat Pools/Temp-2-07.txt > Pools/Quiz-2-04.txt");
		system("cat Pools/Temp-2-08.txt >> Pools/Quiz-2-04.txt");
		if (!question_2_09("Pools/Temp-2-09.txt")) return 0;
		if (!question_2_10("Pools/Temp-2-10.txt")) return 0;
		system("cat Pools/Temp-2-09.txt > Pools/Quiz-2-05.txt");
		system("cat Pools/Temp-2-10.txt >> Pools/Quiz-2-05.txt");
		if (!question_2_11("Pools/Temp-2-11.txt")) return 0;
		if (!question_2_12("Pools/Temp-2-12.txt")) return 0;
		system("cat Pools/Temp-2-11.txt > Pools/Quiz-2-06.txt");
		system("cat Pools/Temp-2-12.txt >> Pools/Quiz-2-06.txt");
		if (!question_2_13("Pools/Temp-2-13.txt")) return 0;
		if (!question_2_14("Pools/Temp-2-14.txt")) return 0;
		system("cat Pools/Temp-2-13.txt > Pools/Quiz-2-07.txt");
		system("cat Pools/Temp-2-14.txt >> Pools/Quiz-2-07.txt");
		if (!question_2_15("Pools/Temp-2-15.txt")) return 0;
		if (!question_2_16("Pools/Temp-2-16.txt")) return 0;
		system("cat Pools/Temp-2-15.txt > Pools/Quiz-2-08.txt");
		system("cat Pools/Temp-2-16.txt >> Pools/Quiz-2-08.txt");
		if (!question_2_17("Pools/Temp-2-17.txt")) return 0;
		if (!question_2_18("Pools/Temp-2-18.txt")) return 0;
		system("cat Pools/Temp-2-17.txt > Pools/Quiz-2-09.txt");
		system("cat Pools/Temp-2-18.txt >> Pools/Quiz-2-09.txt");
		if (!question_2_19("Pools/Temp-2-19.txt")) return 0;
		if (!question_2_20("Pools/Temp-2-20.txt")) return 0;
		system("cat Pools/Temp-2-19.txt > Pools/Quiz-2-10.txt");
		system("cat Pools/Temp-2-20.txt >> Pools/Quiz-2-10.txt");
		if (!question_2_21("Pools/Temp-2-21.txt")) return 0;
		if (!question_2_22("Pools/Temp-2-22.txt")) return 0;
		system("cat Pools/Temp-2-21.txt > Pools/Quiz-2-11.txt");
		system("cat Pools/Temp-2-22.txt >> Pools/Quiz-2-11.txt");
		if (!question_2_23("Pools/Temp-2-23.txt")) return 0;
		if (!question_2_24("Pools/Temp-2-24.txt")) return 0;
		system("cat Pools/Temp-2-23.txt > Pools/Quiz-2-12.txt");
		system("cat Pools/Temp-2-24.txt >> Pools/Quiz-2-12.txt");
		if (!question_2_25("Pools/Temp-2-25.txt")) return 0;
		if (!question_2_26("Pools/Temp-2-26.txt")) return 0;
		system("cat Pools/Temp-2-25.txt > Pools/Quiz-2-13.txt");
		system("cat Pools/Temp-2-26.txt >> Pools/Quiz-2-13.txt");
		if (!question_2_27("Pools/Temp-2-27.txt")) return 0;
		if (!question_2_28("Pools/Temp-2-28.txt")) return 0;
		system("cat Pools/Temp-2-27.txt > Pools/Quiz-2-14.txt");
		system("cat Pools/Temp-2-28.txt >> Pools/Quiz-2-14.txt");
		if (!question_2_29("Pools/Temp-2-29.txt")) return 0;
		if (!question_2_30("Pools/Temp-2-30.txt")) return 0;
		system("cat Pools/Temp-2-29.txt > Pools/Quiz-2-15.txt");
		system("cat Pools/Temp-2-30.txt >> Pools/Quiz-2-15.txt");

		if (!question_3_01("Pools/Temp-3-01.txt")) return 0;
		if (!question_3_02("Pools/Temp-3-02.txt")) return 0;
		system("cat Pools/Temp-3-01.txt > Pools/Quiz-3-01.txt");
		system("cat Pools/Temp-3-02.txt >> Pools/Quiz-3-01.txt");
		if (!question_3_03("Pools/Temp-3-03.txt")) return 0;
		if (!question_3_04("Pools/Temp-3-04.txt")) return 0;
		system("cat Pools/Temp-3-03.txt > Pools/Quiz-3-02.txt");
		system("cat Pools/Temp-3-04.txt >> Pools/Quiz-3-02.txt");
		if (!question_3_05("Pools/Temp-3-05.txt")) return 0;
		if (!question_3_06("Pools/Temp-3-06.txt")) return 0;
		system("cat Pools/Temp-3-05.txt > Pools/Quiz-3-03.txt");
		system("cat Pools/Temp-3-06.txt >> Pools/Quiz-3-03.txt");
		if (!question_3_07("Pools/Temp-3-07.txt")) return 0;
		if (!question_3_08("Pools/Temp-3-08.txt")) return 0;
		system("cat Pools/Temp-3-07.txt > Pools/Quiz-3-04.txt");
		system("cat Pools/Temp-3-08.txt >> Pools/Quiz-3-04.txt");
		if (!question_3_09("Pools/Temp-3-09.txt")) return 0;
		if (!question_3_10("Pools/Temp-3-10.txt")) return 0;
		system("cat Pools/Temp-3-09.txt > Pools/Quiz-3-05.txt");
		system("cat Pools/Temp-3-10.txt >> Pools/Quiz-3-05.txt");
		if (!question_3_11("Pools/Temp-3-11.txt")) return 0;
		if (!question_3_12("Pools/Temp-3-12.txt")) return 0;
		system("cat Pools/Temp-3-11.txt > Pools/Quiz-3-06.txt");
		system("cat Pools/Temp-3-12.txt >> Pools/Quiz-3-06.txt");
		if (!question_3_13("Pools/Temp-3-13.txt")) return 0;
		if (!question_3_14("Pools/Temp-3-14.txt")) return 0;
		system("cat Pools/Temp-3-13.txt > Pools/Quiz-3-07.txt");
		system("cat Pools/Temp-3-14.txt >> Pools/Quiz-3-07.txt");
		if (!question_3_15("Pools/Temp-3-15.txt")) return 0;
		if (!question_3_16("Pools/Temp-3-16.txt")) return 0;
		system("cat Pools/Temp-3-15.txt > Pools/Quiz-3-08.txt");
		system("cat Pools/Temp-3-16.txt >> Pools/Quiz-3-08.txt");
		if (!question_3_17("Pools/Temp-3-17.txt")) return 0;
		if (!question_3_18("Pools/Temp-3-18.txt")) return 0;
		system("cat Pools/Temp-3-17.txt > Pools/Quiz-3-09.txt");
		system("cat Pools/Temp-3-18.txt >> Pools/Quiz-3-09.txt");
		if (!question_3_19("Pools/Temp-3-19.txt")) return 0;
		if (!question_3_20("Pools/Temp-3-20.txt")) return 0;
		system("cat Pools/Temp-3-19.txt > Pools/Quiz-3-10.txt");
		system("cat Pools/Temp-3-20.txt >> Pools/Quiz-3-10.txt");
		if (!question_3_21("Pools/Temp-3-21.txt")) return 0;
		if (!question_3_22("Pools/Temp-3-22.txt")) return 0;
		system("cat Pools/Temp-3-21.txt > Pools/Quiz-3-11.txt");
		system("cat Pools/Temp-3-22.txt >> Pools/Quiz-3-11.txt");
		if (!question_3_23("Pools/Temp-3-23.txt")) return 0;
		if (!question_3_24("Pools/Temp-3-24.txt")) return 0;
		system("cat Pools/Temp-3-23.txt > Pools/Quiz-3-12.txt");
		system("cat Pools/Temp-3-24.txt >> Pools/Quiz-3-12.txt");
		if (!question_3_25("Pools/Temp-3-25.txt")) return 0;
		if (!question_3_26("Pools/Temp-3-26.txt")) return 0;
		system("cat Pools/Temp-3-25.txt > Pools/Quiz-3-13.txt");
		system("cat Pools/Temp-3-26.txt >> Pools/Quiz-3-13.txt");
		if (!question_3_27("Pools/Temp-3-27.txt")) return 0;
		if (!question_3_28("Pools/Temp-3-28.txt")) return 0;
		system("cat Pools/Temp-3-27.txt > Pools/Quiz-3-14.txt");
		system("cat Pools/Temp-3-28.txt >> Pools/Quiz-3-14.txt");
		if (!question_3_29("Pools/Temp-3-29.txt")) return 0;
		if (!question_3_30("Pools/Temp-3-30.txt")) return 0;
		system("cat Pools/Temp-3-29.txt > Pools/Quiz-3-15.txt");
		system("cat Pools/Temp-3-30.txt >> Pools/Quiz-3-15.txt");

		if (!question_4_01("Pools/Temp-4-01.txt")) return 0;
		if (!question_4_02("Pools/Temp-4-02.txt")) return 0;
		system("cat Pools/Temp-4-01.txt > Pools/Quiz-4-01.txt");
		system("cat Pools/Temp-4-02.txt >> Pools/Quiz-4-01.txt");
		if (!question_4_03("Pools/Temp-4-03.txt")) return 0;
		if (!question_4_04("Pools/Temp-4-04.txt")) return 0;
		system("cat Pools/Temp-4-03.txt > Pools/Quiz-4-02.txt");
		system("cat Pools/Temp-4-04.txt >> Pools/Quiz-4-02.txt");
		if (!question_4_05("Pools/Temp-4-05.txt")) return 0;
		if (!question_4_06("Pools/Temp-4-06.txt")) return 0;
		system("cat Pools/Temp-4-05.txt > Pools/Quiz-4-03.txt");
		system("cat Pools/Temp-4-06.txt >> Pools/Quiz-4-03.txt");
		if (!question_4_07("Pools/Temp-4-07.txt")) return 0;
		if (!question_4_08("Pools/Temp-4-08.txt")) return 0;
		system("cat Pools/Temp-4-07.txt > Pools/Quiz-4-04.txt");
		system("cat Pools/Temp-4-08.txt >> Pools/Quiz-4-04.txt");
		if (!question_4_09("Pools/Temp-4-09.txt")) return 0;
		if (!question_4_10("Pools/Temp-4-10.txt")) return 0;
		system("cat Pools/Temp-4-09.txt > Pools/Quiz-4-05.txt");
		system("cat Pools/Temp-4-10.txt >> Pools/Quiz-4-05.txt");
		if (!question_4_11("Pools/Temp-4-11.txt")) return 0;
		if (!question_4_12("Pools/Temp-4-12.txt")) return 0;
		system("cat Pools/Temp-4-11.txt > Pools/Quiz-4-06.txt");
		system("cat Pools/Temp-4-12.txt >> Pools/Quiz-4-06.txt");
		if (!question_4_13("Pools/Temp-4-13.txt")) return 0;
		if (!question_4_14("Pools/Temp-4-14.txt")) return 0;
		system("cat Pools/Temp-4-13.txt > Pools/Quiz-4-07.txt");
		system("cat Pools/Temp-4-14.txt >> Pools/Quiz-4-07.txt");
		if (!question_4_15("Pools/Temp-4-15.txt")) return 0;
		if (!question_4_16("Pools/Temp-4-16.txt")) return 0;
		system("cat Pools/Temp-4-15.txt > Pools/Quiz-4-08.txt");
		system("cat Pools/Temp-4-16.txt >> Pools/Quiz-4-08.txt");
		if (!question_4_17("Pools/Temp-4-17.txt")) return 0;
		if (!question_4_18("Pools/Temp-4-18.txt")) return 0;
		system("cat Pools/Temp-4-17.txt > Pools/Quiz-4-09.txt");
		system("cat Pools/Temp-4-18.txt >> Pools/Quiz-4-09.txt");
		if (!question_4_19("Pools/Temp-4-19.txt")) return 0;
		if (!question_4_20("Pools/Temp-4-20.txt")) return 0;
		system("cat Pools/Temp-4-19.txt > Pools/Quiz-4-10.txt");
		system("cat Pools/Temp-4-20.txt >> Pools/Quiz-4-10.txt");
		if (!question_4_21("Pools/Temp-4-21.txt")) return 0;
		if (!question_4_22("Pools/Temp-4-22.txt")) return 0;
		system("cat Pools/Temp-4-21.txt > Pools/Quiz-4-11.txt");
		system("cat Pools/Temp-4-22.txt >> Pools/Quiz-4-11.txt");
		if (!question_4_23("Pools/Temp-4-23.txt")) return 0;
		if (!question_4_24("Pools/Temp-4-24.txt")) return 0;
		system("cat Pools/Temp-4-23.txt > Pools/Quiz-4-12.txt");
		system("cat Pools/Temp-4-24.txt >> Pools/Quiz-4-12.txt");
		if (!question_4_25("Pools/Temp-4-25.txt")) return 0;
		if (!question_4_26("Pools/Temp-4-26.txt")) return 0;
		system("cat Pools/Temp-4-25.txt > Pools/Quiz-4-13.txt");
		system("cat Pools/Temp-4-26.txt >> Pools/Quiz-4-13.txt");
		if (!question_4_27("Pools/Temp-4-27.txt")) return 0;
		if (!question_4_28("Pools/Temp-4-28.txt")) return 0;
		system("cat Pools/Temp-4-27.txt > Pools/Quiz-4-14.txt");
		system("cat Pools/Temp-4-28.txt >> Pools/Quiz-4-14.txt");
		if (!question_4_29("Pools/Temp-4-29.txt")) return 0;
		if (!question_4_30("Pools/Temp-4-30.txt")) return 0;
		system("cat Pools/Temp-4-29.txt > Pools/Quiz-4-15.txt");
		system("cat Pools/Temp-4-30.txt >> Pools/Quiz-4-15.txt");

		if (!question_5_01("Pools/Temp-5-01.txt")) return 0;
		if (!question_5_02("Pools/Temp-5-02.txt")) return 0;
		system("cat Pools/Temp-5-01.txt > Pools/Quiz-5-01.txt");
		system("cat Pools/Temp-5-02.txt >> Pools/Quiz-5-01.txt");
		if (!question_5_03("Pools/Temp-5-03.txt")) return 0;
		if (!question_5_04("Pools/Temp-5-04.txt")) return 0;
		system("cat Pools/Temp-5-03.txt > Pools/Quiz-5-02.txt");
		system("cat Pools/Temp-5-04.txt >> Pools/Quiz-5-02.txt");
		if (!question_5_05("Pools/Temp-5-05.txt")) return 0;
		if (!question_5_06("Pools/Temp-5-06.txt")) return 0;
		system("cat Pools/Temp-5-05.txt > Pools/Quiz-5-03.txt");
		system("cat Pools/Temp-5-06.txt >> Pools/Quiz-5-03.txt");
		if (!question_5_07("Pools/Temp-5-07.txt")) return 0;
		if (!question_5_08("Pools/Temp-5-08.txt")) return 0;
		system("cat Pools/Temp-5-07.txt > Pools/Quiz-5-04.txt");
		system("cat Pools/Temp-5-08.txt >> Pools/Quiz-5-04.txt");
		if (!question_5_09("Pools/Temp-5-09.txt")) return 0;
		if (!question_5_10("Pools/Temp-5-10.txt")) return 0;
		system("cat Pools/Temp-5-09.txt > Pools/Quiz-5-05.txt");
		system("cat Pools/Temp-5-10.txt >> Pools/Quiz-5-05.txt");
		if (!question_5_11("Pools/Temp-5-11.txt")) return 0;
		if (!question_5_12("Pools/Temp-5-12.txt")) return 0;
		system("cat Pools/Temp-5-11.txt > Pools/Quiz-5-06.txt");
		system("cat Pools/Temp-5-12.txt >> Pools/Quiz-5-06.txt");
		if (!question_5_13("Pools/Temp-5-13.txt")) return 0;
		if (!question_5_14("Pools/Temp-5-14.txt")) return 0;
		system("cat Pools/Temp-5-13.txt > Pools/Quiz-5-07.txt");
		system("cat Pools/Temp-5-14.txt >> Pools/Quiz-5-07.txt");
		if (!question_5_15("Pools/Temp-5-15.txt")) return 0;
		if (!question_5_16("Pools/Temp-5-16.txt")) return 0;
		system("cat Pools/Temp-5-15.txt > Pools/Quiz-5-08.txt");
		system("cat Pools/Temp-5-16.txt >> Pools/Quiz-5-08.txt");
		if (!question_5_17("Pools/Temp-5-17.txt")) return 0;
		if (!question_5_18("Pools/Temp-5-18.txt")) return 0;
		system("cat Pools/Temp-5-17.txt > Pools/Quiz-5-09.txt");
		system("cat Pools/Temp-5-18.txt >> Pools/Quiz-5-09.txt");
		if (!question_5_19("Pools/Temp-5-19.txt")) return 0;
		if (!question_5_20("Pools/Temp-5-20.txt")) return 0;
		system("cat Pools/Temp-5-19.txt > Pools/Quiz-5-10.txt");
		system("cat Pools/Temp-5-20.txt >> Pools/Quiz-5-10.txt");
		if (!question_5_21("Pools/Temp-5-21.txt")) return 0;
		if (!question_5_22("Pools/Temp-5-22.txt")) return 0;
		system("cat Pools/Temp-5-21.txt > Pools/Quiz-5-11.txt");
		system("cat Pools/Temp-5-22.txt >> Pools/Quiz-5-11.txt");
		if (!question_5_23("Pools/Temp-5-23.txt")) return 0;
		if (!question_5_24("Pools/Temp-5-24.txt")) return 0;
		system("cat Pools/Temp-5-23.txt > Pools/Quiz-5-12.txt");
		system("cat Pools/Temp-5-24.txt >> Pools/Quiz-5-12.txt");
		if (!question_5_25("Pools/Temp-5-25.txt")) return 0;
		if (!question_5_26("Pools/Temp-5-26.txt")) return 0;
		system("cat Pools/Temp-5-25.txt > Pools/Quiz-5-13.txt");
		system("cat Pools/Temp-5-26.txt >> Pools/Quiz-5-13.txt");
		if (!question_5_27("Pools/Temp-5-27.txt")) return 0;
		if (!question_5_28("Pools/Temp-5-28.txt")) return 0;
		system("cat Pools/Temp-5-27.txt > Pools/Quiz-5-14.txt");
		system("cat Pools/Temp-5-28.txt >> Pools/Quiz-5-14.txt");
	
		if (!question_6_01("Pools/Temp-6-01.txt")) return 0;
		if (!question_6_02("Pools/Temp-6-02.txt")) return 0;
		system("cat Pools/Temp-6-01.txt > Pools/Quiz-6-01.txt");
		system("cat Pools/Temp-6-02.txt >> Pools/Quiz-6-01.txt");
		if (!question_6_03("Pools/Temp-6-03.txt")) return 0;
		if (!question_6_04("Pools/Temp-6-04.txt")) return 0;
		system("cat Pools/Temp-6-03.txt > Pools/Quiz-6-02.txt");
		system("cat Pools/Temp-6-04.txt >> Pools/Quiz-6-02.txt");
		if (!question_6_05("Pools/Temp-6-05.txt")) return 0;
		if (!question_6_06("Pools/Temp-6-06.txt")) return 0;
		system("cat Pools/Temp-6-05.txt > Pools/Quiz-6-03.txt");
		system("cat Pools/Temp-6-06.txt >> Pools/Quiz-6-03.txt");


	
		if (!question_1_01("Pools/Temp-1-01.txt")) return 0;
		if (!question_1_02("Pools/Temp-1-02.txt")) return 0;
		system("cat Pools/Temp-1-01.txt > Pools/Exam-1-01.txt");
		system("cat Pools/Temp-1-02.txt >> Pools/Exam-1-01.txt");
		if (!question_1_03("Pools/Temp-1-03.txt")) return 0;
		if (!question_1_04("Pools/Temp-1-04.txt")) return 0;
		if (!question_1_05("Pools/Temp-1-05.txt")) return 0;
		if (!question_1_06("Pools/Temp-1-06.txt")) return 0;
		if (!question_1_07("Pools/Temp-1-07.txt")) return 0;
		system("cat Pools/Temp-1-03.txt > Pools/Exam-1-02.txt");
		system("cat Pools/Temp-1-04.txt >> Pools/Exam-1-02.txt");
		system("cat Pools/Temp-1-05.txt >> Pools/Exam-1-02.txt");
		system("cat Pools/Temp-1-06.txt >> Pools/Exam-1-02.txt");
		system("cat Pools/Temp-1-07.txt >> Pools/Exam-1-02.txt");
		if (!question_1_08("Pools/Temp-1-08.txt")) return 0;
		if (!question_1_09("Pools/Temp-1-09.txt")) return 0;
		if (!question_1_10("Pools/Temp-1-10.txt")) return 0;
		system("cat Pools/Temp-1-08.txt > Pools/Exam-1-03.txt");
		system("cat Pools/Temp-1-09.txt >> Pools/Exam-1-03.txt");
		system("cat Pools/Temp-1-10.txt >> Pools/Exam-1-03.txt");				
		if (!question_1_11("Pools/Temp-1-11.txt")) return 0;
		if (!question_1_12("Pools/Temp-1-12.txt")) return 0;
		if (!question_1_13("Pools/Temp-1-13.txt")) return 0;
		system("cat Pools/Temp-1-11.txt > Pools/Exam-1-04.txt");
		system("cat Pools/Temp-1-12.txt >> Pools/Exam-1-04.txt");
		system("cat Pools/Temp-1-13.txt >> Pools/Exam-1-04.txt");		
		if (!question_1_14("Pools/Temp-1-14.txt")) return 0;
		if (!question_1_15("Pools/Temp-1-15.txt")) return 0;
		if (!question_1_16("Pools/Temp-1-16.txt")) return 0;
		system("cat Pools/Temp-1-14.txt > Pools/Exam-1-05.txt");
		system("cat Pools/Temp-1-15.txt >> Pools/Exam-1-05.txt");
		system("cat Pools/Temp-1-16.txt >> Pools/Exam-1-05.txt");
		if (!question_1_17("Pools/Temp-1-17.txt")) return 0;
		if (!question_1_18("Pools/Temp-1-18.txt")) return 0;
		if (!question_1_19("Pools/Temp-1-19.txt")) return 0;
		system("cat Pools/Temp-1-17.txt > Pools/Exam-1-06.txt");
		system("cat Pools/Temp-1-18.txt >> Pools/Exam-1-06.txt");
		system("cat Pools/Temp-1-19.txt >> Pools/Exam-1-06.txt");
		if (!question_1_20("Pools/Temp-1-20.txt")) return 0;
		if (!question_1_21("Pools/Temp-1-21.txt")) return 0;
		if (!question_1_22("Pools/Temp-1-22.txt")) return 0;
		system("cat Pools/Temp-1-20.txt > Pools/Exam-1-07.txt");
		system("cat Pools/Temp-1-21.txt >> Pools/Exam-1-07.txt");
		system("cat Pools/Temp-1-22.txt >> Pools/Exam-1-07.txt");
		if (!question_1_23("Pools/Temp-1-23.txt")) return 0;
		if (!question_1_24("Pools/Temp-1-24.txt")) return 0;
		if (!question_1_25("Pools/Temp-1-25.txt")) return 0;
		system("cat Pools/Temp-1-23.txt > Pools/Exam-1-08.txt");
		system("cat Pools/Temp-1-24.txt >> Pools/Exam-1-08.txt");
		system("cat Pools/Temp-1-25.txt >> Pools/Exam-1-08.txt");		
		if (!question_1_26("Pools/Temp-1-26.txt")) return 0;
		if (!question_1_27("Pools/Temp-1-27.txt")) return 0;
		system("cat Pools/Temp-1-26.txt > Pools/Exam-1-09.txt");
		system("cat Pools/Temp-1-27.txt >> Pools/Exam-1-09.txt");
		if (!question_1_28("Pools/Temp-1-28.txt")) return 0;
		if (!question_1_29("Pools/Temp-1-29.txt")) return 0;
		if (!question_1_30("Pools/Temp-1-30.txt")) return 0;
		system("cat Pools/Temp-1-28.txt > Pools/Exam-1-10.txt");
		system("cat Pools/Temp-1-29.txt >> Pools/Exam-1-10.txt");
		system("cat Pools/Temp-1-30.txt >> Pools/Exam-1-10.txt");

		if (!question_2_01("Pools/Temp-2-01.txt")) return 0;
		if (!question_2_02("Pools/Temp-2-02.txt")) return 0;
		if (!question_2_03("Pools/Temp-2-03.txt")) return 0;
		if (!question_2_04("Pools/Temp-2-04.txt")) return 0;
		system("cat Pools/Temp-2-01.txt > Pools/Exam-1-11.txt");
		system("cat Pools/Temp-2-02.txt >> Pools/Exam-1-11.txt");
		system("cat Pools/Temp-2-03.txt >> Pools/Exam-1-11.txt");
		system("cat Pools/Temp-2-04.txt >> Pools/Exam-1-11.txt");
		if (!question_2_05("Pools/Temp-2-05.txt")) return 0;
		if (!question_2_06("Pools/Temp-2-06.txt")) return 0;
		if (!question_2_07("Pools/Temp-2-07.txt")) return 0;
		if (!question_2_08("Pools/Temp-2-08.txt")) return 0;
		system("cat Pools/Temp-2-05.txt > Pools/Exam-1-12.txt");
		system("cat Pools/Temp-2-06.txt >> Pools/Exam-1-12.txt");
		system("cat Pools/Temp-2-07.txt >> Pools/Exam-1-12.txt");
		system("cat Pools/Temp-2-08.txt >> Pools/Exam-1-12.txt");
		if (!question_2_09("Pools/Temp-2-09.txt")) return 0;
		system("cat Pools/Temp-2-09.txt > Pools/Exam-1-13.txt");
		if (!question_2_10("Pools/Temp-2-10.txt")) return 0;
		if (!question_2_11("Pools/Temp-2-11.txt")) return 0;
		system("cat Pools/Temp-2-10.txt > Pools/Exam-1-14.txt");
		system("cat Pools/Temp-2-11.txt >> Pools/Exam-1-14.txt");
		if (!question_2_12("Pools/Temp-2-12.txt")) return 0;
		if (!question_2_13("Pools/Temp-2-13.txt")) return 0;
		system("cat Pools/Temp-2-12.txt > Pools/Exam-1-15.txt");
		system("cat Pools/Temp-2-13.txt >> Pools/Exam-1-15.txt");
		if (!question_2_14("Pools/Temp-2-14.txt")) return 0;
		if (!question_2_15("Pools/Temp-2-15.txt")) return 0;
		if (!question_2_16("Pools/Temp-2-16.txt")) return 0;
		if (!question_2_17("Pools/Temp-2-17.txt")) return 0;
		system("cat Pools/Temp-2-14.txt > Pools/Exam-1-16.txt");
		system("cat Pools/Temp-2-15.txt >> Pools/Exam-1-16.txt");
		system("cat Pools/Temp-2-16.txt >> Pools/Exam-1-16.txt");
		system("cat Pools/Temp-2-17.txt >> Pools/Exam-1-16.txt");
		if (!question_2_18("Pools/Temp-2-18.txt")) return 0;
		if (!question_2_19("Pools/Temp-2-19.txt")) return 0;
		system("cat Pools/Temp-2-18.txt > Pools/Exam-1-17.txt");
		system("cat Pools/Temp-2-19.txt >> Pools/Exam-1-17.txt");
		if (!question_2_20("Pools/Temp-2-20.txt")) return 0;
		if (!question_2_21("Pools/Temp-2-21.txt")) return 0;
		if (!question_2_22("Pools/Temp-2-22.txt")) return 0;
		if (!question_2_23("Pools/Temp-2-23.txt")) return 0;
		if (!question_2_24("Pools/Temp-2-24.txt")) return 0;
		if (!question_2_25("Pools/Temp-2-25.txt")) return 0;
		system("cat Pools/Temp-2-20.txt > Pools/Exam-1-18.txt");
		system("cat Pools/Temp-2-21.txt >> Pools/Exam-1-18.txt");
		system("cat Pools/Temp-2-22.txt >> Pools/Exam-1-18.txt");
		system("cat Pools/Temp-2-23.txt >> Pools/Exam-1-18.txt");
		system("cat Pools/Temp-2-24.txt >> Pools/Exam-1-18.txt");
		system("cat Pools/Temp-2-25.txt >> Pools/Exam-1-18.txt");
		if (!question_2_26("Pools/Temp-2-26.txt")) return 0;
		if (!question_2_27("Pools/Temp-2-27.txt")) return 0;
		if (!question_2_28("Pools/Temp-2-28.txt")) return 0;
		system("cat Pools/Temp-2-26.txt > Pools/Exam-1-19.txt");
		system("cat Pools/Temp-2-27.txt >> Pools/Exam-1-19.txt");
		system("cat Pools/Temp-2-28.txt >> Pools/Exam-1-19.txt");
		if (!question_2_29("Pools/Temp-2-29.txt")) return 0;
		if (!question_2_30("Pools/Temp-2-30.txt")) return 0;
		system("cat Pools/Temp-2-29.txt > Pools/Exam-1-20.txt");
		system("cat Pools/Temp-2-30.txt >> Pools/Exam-1-20.txt");

		if (!question_3_01("Pools/Temp-3-01.txt")) return 0;
		if (!question_3_02("Pools/Temp-3-02.txt")) return 0;
		system("cat Pools/Temp-3-01.txt > Pools/Exam-2-01.txt");
		system("cat Pools/Temp-3-02.txt >> Pools/Exam-2-01.txt");
		if (!question_3_03("Pools/Temp-3-03.txt")) return 0;
		if (!question_3_04("Pools/Temp-3-04.txt")) return 0;
		if (!question_3_05("Pools/Temp-3-05.txt")) return 0;
		if (!question_3_06("Pools/Temp-3-06.txt")) return 0;
		system("cat Pools/Temp-3-03.txt > Pools/Exam-2-02.txt");
		system("cat Pools/Temp-3-04.txt >> Pools/Exam-2-02.txt");
		system("cat Pools/Temp-3-05.txt >> Pools/Exam-2-02.txt");
		system("cat Pools/Temp-3-06.txt >> Pools/Exam-2-02.txt");
		if (!question_3_07("Pools/Temp-3-07.txt")) return 0;
		if (!question_3_08("Pools/Temp-3-08.txt")) return 0;
		system("cat Pools/Temp-3-07.txt > Pools/Exam-2-03.txt");
		system("cat Pools/Temp-3-08.txt >> Pools/Exam-2-03.txt");
		if (!question_3_09("Pools/Temp-3-09.txt")) return 0;
		if (!question_3_10("Pools/Temp-3-10.txt")) return 0;
		if (!question_3_11("Pools/Temp-3-11.txt")) return 0;
		system("cat Pools/Temp-3-09.txt > Pools/Exam-2-04.txt");
		system("cat Pools/Temp-3-10.txt >> Pools/Exam-2-04.txt");
		system("cat Pools/Temp-3-11.txt >> Pools/Exam-2-04.txt");
		if (!question_3_12("Pools/Temp-3-12.txt")) return 0;
		if (!question_3_13("Pools/Temp-3-13.txt")) return 0;
		system("cat Pools/Temp-3-12.txt > Pools/Exam-2-05.txt");
		system("cat Pools/Temp-3-13.txt >> Pools/Exam-2-05.txt");
		if (!question_3_14("Pools/Temp-3-14.txt")) return 0;
		if (!question_3_15("Pools/Temp-3-15.txt")) return 0;
		if (!question_3_16("Pools/Temp-3-16.txt")) return 0;
		system("cat Pools/Temp-3-14.txt > Pools/Exam-2-06.txt");
		system("cat Pools/Temp-3-15.txt >> Pools/Exam-2-06.txt");
		system("cat Pools/Temp-3-16.txt >> Pools/Exam-2-06.txt");
		if (!question_3_17("Pools/Temp-3-17.txt")) return 0;
		if (!question_3_18("Pools/Temp-3-18.txt")) return 0;
		system("cat Pools/Temp-3-17.txt > Pools/Exam-2-07.txt");
		system("cat Pools/Temp-3-18.txt >> Pools/Exam-2-07.txt");
		if (!question_3_19("Pools/Temp-3-19.txt")) return 0;
		if (!question_3_20("Pools/Temp-3-20.txt")) return 0;
		if (!question_3_21("Pools/Temp-3-21.txt")) return 0;
		if (!question_3_22("Pools/Temp-3-22.txt")) return 0;
		system("cat Pools/Temp-3-19.txt > Pools/Exam-2-08.txt");
		system("cat Pools/Temp-3-20.txt >> Pools/Exam-2-08.txt");
		system("cat Pools/Temp-3-21.txt >> Pools/Exam-2-08.txt");
		system("cat Pools/Temp-3-22.txt >> Pools/Exam-2-08.txt");
		if (!question_3_23("Pools/Temp-3-23.txt")) return 0;
		if (!question_3_24("Pools/Temp-3-24.txt")) return 0;
		system("cat Pools/Temp-3-23.txt > Pools/Exam-2-09.txt");
		system("cat Pools/Temp-3-24.txt >> Pools/Exam-2-09.txt");
		if (!question_3_25("Pools/Temp-3-25.txt")) return 0;
		if (!question_3_26("Pools/Temp-3-26.txt")) return 0;
		if (!question_3_27("Pools/Temp-3-27.txt")) return 0;
		if (!question_3_28("Pools/Temp-3-28.txt")) return 0;
		if (!question_3_29("Pools/Temp-3-29.txt")) return 0;
		if (!question_3_30("Pools/Temp-3-30.txt")) return 0;
		system("cat Pools/Temp-3-25.txt > Pools/Exam-2-10.txt");
		system("cat Pools/Temp-3-26.txt >> Pools/Exam-2-10.txt");
		system("cat Pools/Temp-3-27.txt >> Pools/Exam-2-10.txt");
		system("cat Pools/Temp-3-28.txt >> Pools/Exam-2-10.txt");
		system("cat Pools/Temp-3-29.txt >> Pools/Exam-2-10.txt");
		system("cat Pools/Temp-3-30.txt >> Pools/Exam-2-10.txt");

		if (!question_4_01("Pools/Temp-4-01.txt")) return 0;
		if (!question_4_02("Pools/Temp-4-02.txt")) return 0;
		if (!question_4_03("Pools/Temp-4-03.txt")) return 0;
		if (!question_4_04("Pools/Temp-4-04.txt")) return 0;
		system("cat Pools/Temp-4-01.txt > Pools/Exam-2-11.txt");
		system("cat Pools/Temp-4-02.txt >> Pools/Exam-2-11.txt");
		system("cat Pools/Temp-4-03.txt >> Pools/Exam-2-11.txt");
		system("cat Pools/Temp-4-04.txt >> Pools/Exam-2-11.txt");
		if (!question_4_05("Pools/Temp-4-05.txt")) return 0;
		if (!question_4_06("Pools/Temp-4-06.txt")) return 0;
		system("cat Pools/Temp-4-05.txt > Pools/Exam-2-12.txt");
		system("cat Pools/Temp-4-06.txt >> Pools/Exam-2-12.txt");
		if (!question_4_07("Pools/Temp-4-07.txt")) return 0;
		if (!question_4_08("Pools/Temp-4-08.txt")) return 0;
		system("cat Pools/Temp-4-07.txt > Pools/Exam-2-13.txt");
		system("cat Pools/Temp-4-08.txt >> Pools/Exam-2-13.txt");
		if (!question_4_09("Pools/Temp-4-09.txt")) return 0;
		if (!question_4_10("Pools/Temp-4-10.txt")) return 0;
		if (!question_4_11("Pools/Temp-4-11.txt")) return 0;
		if (!question_4_12("Pools/Temp-4-12.txt")) return 0;
		system("cat Pools/Temp-4-09.txt > Pools/Exam-2-14.txt");
		system("cat Pools/Temp-4-10.txt >> Pools/Exam-2-14.txt");
		system("cat Pools/Temp-4-11.txt >> Pools/Exam-2-14.txt");
		system("cat Pools/Temp-4-12.txt >> Pools/Exam-2-14.txt");
		if (!question_4_13("Pools/Temp-4-13.txt")) return 0;
		if (!question_4_14("Pools/Temp-4-14.txt")) return 0;
		if (!question_4_15("Pools/Temp-4-15.txt")) return 0;
		if (!question_4_16("Pools/Temp-4-16.txt")) return 0;
		if (!question_4_17("Pools/Temp-4-17.txt")) return 0;
		system("cat Pools/Temp-4-13.txt > Pools/Exam-2-15.txt");
		system("cat Pools/Temp-4-14.txt >> Pools/Exam-2-15.txt");
		system("cat Pools/Temp-4-15.txt >> Pools/Exam-2-15.txt");
		system("cat Pools/Temp-4-16.txt >> Pools/Exam-2-15.txt");
		system("cat Pools/Temp-4-17.txt >> Pools/Exam-2-15.txt");
		if (!question_4_18("Pools/Temp-4-18.txt")) return 0;
		system("cat Pools/Temp-4-18.txt > Pools/Exam-2-16.txt");
		if (!question_4_19("Pools/Temp-4-19.txt")) return 0;
		if (!question_4_20("Pools/Temp-4-20.txt")) return 0;
		if (!question_4_21("Pools/Temp-4-21.txt")) return 0;
		system("cat Pools/Temp-4-19.txt > Pools/Exam-2-17.txt");
		system("cat Pools/Temp-4-20.txt >> Pools/Exam-2-17.txt");
		system("cat Pools/Temp-4-21.txt >> Pools/Exam-2-17.txt");
		if (!question_4_22("Pools/Temp-4-22.txt")) return 0;
		if (!question_4_23("Pools/Temp-4-23.txt")) return 0;
		if (!question_4_24("Pools/Temp-4-24.txt")) return 0;
		system("cat Pools/Temp-4-22.txt > Pools/Exam-2-18.txt");
		system("cat Pools/Temp-4-23.txt >> Pools/Exam-2-18.txt");
		system("cat Pools/Temp-4-24.txt >> Pools/Exam-2-18.txt");
		if (!question_4_25("Pools/Temp-4-25.txt")) return 0;
		if (!question_4_26("Pools/Temp-4-26.txt")) return 0;
		if (!question_4_27("Pools/Temp-4-27.txt")) return 0;
		system("cat Pools/Temp-4-25.txt > Pools/Exam-2-19.txt");
		system("cat Pools/Temp-4-26.txt >> Pools/Exam-2-19.txt");
		system("cat Pools/Temp-4-27.txt >> Pools/Exam-2-19.txt");
		if (!question_4_28("Pools/Temp-4-28.txt")) return 0;
		if (!question_4_29("Pools/Temp-4-29.txt")) return 0;
		if (!question_4_30("Pools/Temp-4-30.txt")) return 0;
		system("cat Pools/Temp-4-28.txt > Pools/Exam-2-20.txt");
		system("cat Pools/Temp-4-29.txt >> Pools/Exam-2-20.txt");
		system("cat Pools/Temp-4-30.txt >> Pools/Exam-2-20.txt");

		if (!question_1_01("Pools/Temp-1-01.txt")) return 0;
		if (!question_1_02("Pools/Temp-1-02.txt")) return 0;
		if (!question_1_03("Pools/Temp-1-03.txt")) return 0;
		if (!question_1_04("Pools/Temp-1-04.txt")) return 0;
		if (!question_1_05("Pools/Temp-1-05.txt")) return 0;
		if (!question_1_06("Pools/Temp-1-06.txt")) return 0;
		if (!question_1_07("Pools/Temp-1-07.txt")) return 0;
		system("cat Pools/Temp-1-01.txt > Pools/Exam-3-01.txt");
		system("cat Pools/Temp-1-02.txt >> Pools/Exam-3-01.txt");
		system("cat Pools/Temp-1-03.txt >> Pools/Exam-3-01.txt");
		system("cat Pools/Temp-1-04.txt >> Pools/Exam-3-01.txt");
		system("cat Pools/Temp-1-05.txt >> Pools/Exam-3-01.txt");
		system("cat Pools/Temp-1-06.txt >> Pools/Exam-3-01.txt");
		system("cat Pools/Temp-1-07.txt >> Pools/Exam-3-01.txt");
		if (!question_1_08("Pools/Temp-1-08.txt")) return 0;
		if (!question_1_09("Pools/Temp-1-09.txt")) return 0;
		if (!question_1_10("Pools/Temp-1-10.txt")) return 0;
		if (!question_1_11("Pools/Temp-1-11.txt")) return 0;
		if (!question_1_12("Pools/Temp-1-12.txt")) return 0;
		if (!question_1_13("Pools/Temp-1-13.txt")) return 0;
		if (!question_1_14("Pools/Temp-1-14.txt")) return 0;
		if (!question_1_15("Pools/Temp-1-15.txt")) return 0;
		if (!question_1_16("Pools/Temp-1-16.txt")) return 0;
		if (!question_1_17("Pools/Temp-1-17.txt")) return 0;
		if (!question_1_18("Pools/Temp-1-18.txt")) return 0;
		if (!question_1_19("Pools/Temp-1-19.txt")) return 0;
		system("cat Pools/Temp-1-08.txt > Pools/Exam-3-02.txt");
		system("cat Pools/Temp-1-09.txt >> Pools/Exam-3-02.txt");
		system("cat Pools/Temp-1-10.txt >> Pools/Exam-3-02.txt");				
		system("cat Pools/Temp-1-11.txt >> Pools/Exam-3-02.txt");
		system("cat Pools/Temp-1-12.txt >> Pools/Exam-3-02.txt");
		system("cat Pools/Temp-1-13.txt >> Pools/Exam-3-02.txt");		
		system("cat Pools/Temp-1-14.txt >> Pools/Exam-3-02.txt");
		system("cat Pools/Temp-1-15.txt >> Pools/Exam-3-02.txt");
		system("cat Pools/Temp-1-16.txt >> Pools/Exam-3-02.txt");
		system("cat Pools/Temp-1-17.txt >> Pools/Exam-3-02.txt");
		system("cat Pools/Temp-1-18.txt >> Pools/Exam-3-02.txt");
		system("cat Pools/Temp-1-19.txt >> Pools/Exam-3-02.txt");
		if (!question_1_20("Pools/Temp-1-20.txt")) return 0;
		if (!question_1_21("Pools/Temp-1-21.txt")) return 0;
		if (!question_1_22("Pools/Temp-1-22.txt")) return 0;
		if (!question_1_23("Pools/Temp-1-23.txt")) return 0;
		if (!question_1_24("Pools/Temp-1-24.txt")) return 0;
		if (!question_1_25("Pools/Temp-1-25.txt")) return 0;
		system("cat Pools/Temp-1-20.txt > Pools/Exam-3-03.txt");
		system("cat Pools/Temp-1-21.txt >> Pools/Exam-3-03.txt");
		system("cat Pools/Temp-1-22.txt >> Pools/Exam-3-03.txt");
		system("cat Pools/Temp-1-23.txt >> Pools/Exam-3-03.txt");
		system("cat Pools/Temp-1-24.txt >> Pools/Exam-3-03.txt");
		system("cat Pools/Temp-1-25.txt >> Pools/Exam-3-03.txt");
		if (!question_1_26("Pools/Temp-1-26.txt")) return 0;
		if (!question_1_27("Pools/Temp-1-27.txt")) return 0;
		if (!question_1_28("Pools/Temp-1-28.txt")) return 0;
		if (!question_1_29("Pools/Temp-1-29.txt")) return 0;
		if (!question_1_30("Pools/Temp-1-30.txt")) return 0;
		system("cat Pools/Temp-1-26.txt > Pools/Exam-3-04.txt");
		system("cat Pools/Temp-1-27.txt >> Pools/Exam-3-04.txt");
		system("cat Pools/Temp-1-28.txt >> Pools/Exam-3-04.txt");
		system("cat Pools/Temp-1-29.txt >> Pools/Exam-3-04.txt");
		system("cat Pools/Temp-1-30.txt >> Pools/Exam-3-04.txt");

		if (!question_2_01("Pools/Temp-2-01.txt")) return 0;
		if (!question_2_02("Pools/Temp-2-02.txt")) return 0;
		if (!question_2_03("Pools/Temp-2-03.txt")) return 0;
		if (!question_2_04("Pools/Temp-2-04.txt")) return 0;
		system("cat Pools/Temp-2-01.txt > Pools/Exam-3-05.txt");
		system("cat Pools/Temp-2-02.txt >> Pools/Exam-3-05.txt");
		system("cat Pools/Temp-2-03.txt >> Pools/Exam-3-05.txt");
		system("cat Pools/Temp-2-04.txt >> Pools/Exam-3-05.txt");
		if (!question_2_05("Pools/Temp-2-05.txt")) return 0;
		if (!question_2_06("Pools/Temp-2-06.txt")) return 0;
		if (!question_2_07("Pools/Temp-2-07.txt")) return 0;
		if (!question_2_08("Pools/Temp-2-08.txt")) return 0;
		if (!question_2_09("Pools/Temp-2-09.txt")) return 0;
		system("cat Pools/Temp-2-05.txt > Pools/Exam-3-06.txt");
		system("cat Pools/Temp-2-06.txt >> Pools/Exam-3-06.txt");
		system("cat Pools/Temp-2-07.txt >> Pools/Exam-3-06.txt");
		system("cat Pools/Temp-2-08.txt >> Pools/Exam-3-06.txt");
		system("cat Pools/Temp-2-09.txt >> Pools/Exam-3-06.txt");
		if (!question_2_10("Pools/Temp-2-10.txt")) return 0;
		if (!question_2_11("Pools/Temp-2-11.txt")) return 0;
		if (!question_2_12("Pools/Temp-2-12.txt")) return 0;
		if (!question_2_13("Pools/Temp-2-13.txt")) return 0;
		system("cat Pools/Temp-2-10.txt > Pools/Exam-3-07.txt");
		system("cat Pools/Temp-2-11.txt >> Pools/Exam-3-07.txt");
		system("cat Pools/Temp-2-12.txt >> Pools/Exam-3-07.txt");
		system("cat Pools/Temp-2-13.txt >> Pools/Exam-3-07.txt");
		if (!question_2_14("Pools/Temp-2-14.txt")) return 0;
		if (!question_2_15("Pools/Temp-2-15.txt")) return 0;
		if (!question_2_16("Pools/Temp-2-16.txt")) return 0;
		if (!question_2_17("Pools/Temp-2-17.txt")) return 0;
		system("cat Pools/Temp-2-14.txt > Pools/Exam-3-08.txt");
		system("cat Pools/Temp-2-15.txt >> Pools/Exam-3-08.txt");
		system("cat Pools/Temp-2-16.txt >> Pools/Exam-3-08.txt");
		system("cat Pools/Temp-2-17.txt >> Pools/Exam-3-08.txt");
		if (!question_2_18("Pools/Temp-2-18.txt")) return 0;
		if (!question_2_19("Pools/Temp-2-19.txt")) return 0;
		if (!question_2_20("Pools/Temp-2-20.txt")) return 0;
		if (!question_2_21("Pools/Temp-2-21.txt")) return 0;
		if (!question_2_22("Pools/Temp-2-22.txt")) return 0;
		if (!question_2_23("Pools/Temp-2-23.txt")) return 0;
		if (!question_2_24("Pools/Temp-2-24.txt")) return 0;
		if (!question_2_25("Pools/Temp-2-25.txt")) return 0;
		system("cat Pools/Temp-2-18.txt > Pools/Exam-3-09.txt");
		system("cat Pools/Temp-2-19.txt >> Pools/Exam-3-09.txt");
		system("cat Pools/Temp-2-20.txt >> Pools/Exam-3-09.txt");
		system("cat Pools/Temp-2-21.txt >> Pools/Exam-3-09.txt");
		system("cat Pools/Temp-2-22.txt >> Pools/Exam-3-09.txt");
		system("cat Pools/Temp-2-23.txt >> Pools/Exam-3-09.txt");
		system("cat Pools/Temp-2-24.txt >> Pools/Exam-3-09.txt");
		system("cat Pools/Temp-2-25.txt >> Pools/Exam-3-09.txt");
		if (!question_2_26("Pools/Temp-2-26.txt")) return 0;
		if (!question_2_27("Pools/Temp-2-27.txt")) return 0;
		if (!question_2_28("Pools/Temp-2-28.txt")) return 0;
		if (!question_2_29("Pools/Temp-2-29.txt")) return 0;
		if (!question_2_30("Pools/Temp-2-30.txt")) return 0;
		system("cat Pools/Temp-2-26.txt > Pools/Exam-3-10.txt");
		system("cat Pools/Temp-2-27.txt >> Pools/Exam-3-10.txt");
		system("cat Pools/Temp-2-28.txt >> Pools/Exam-3-10.txt");
		system("cat Pools/Temp-2-29.txt >> Pools/Exam-3-10.txt");
		system("cat Pools/Temp-2-30.txt >> Pools/Exam-3-10.txt");
		
		if (!question_3_01("Pools/Temp-3-01.txt")) return 0;
		if (!question_3_02("Pools/Temp-3-02.txt")) return 0;
		if (!question_3_03("Pools/Temp-3-03.txt")) return 0;
		if (!question_3_04("Pools/Temp-3-04.txt")) return 0;
		if (!question_3_05("Pools/Temp-3-05.txt")) return 0;
		if (!question_3_06("Pools/Temp-3-06.txt")) return 0;
		system("cat Pools/Temp-3-01.txt > Pools/Exam-3-11.txt");
		system("cat Pools/Temp-3-02.txt >> Pools/Exam-3-11.txt");
		system("cat Pools/Temp-3-03.txt >> Pools/Exam-3-11.txt");
		system("cat Pools/Temp-3-04.txt >> Pools/Exam-3-11.txt");
		system("cat Pools/Temp-3-05.txt >> Pools/Exam-3-11.txt");
		system("cat Pools/Temp-3-06.txt >> Pools/Exam-3-11.txt");
		if (!question_3_07("Pools/Temp-3-07.txt")) return 0;
		if (!question_3_08("Pools/Temp-3-08.txt")) return 0;
		if (!question_3_09("Pools/Temp-3-09.txt")) return 0;
		if (!question_3_10("Pools/Temp-3-10.txt")) return 0;
		if (!question_3_11("Pools/Temp-3-11.txt")) return 0;
		system("cat Pools/Temp-3-07.txt > Pools/Exam-3-12.txt");
		system("cat Pools/Temp-3-08.txt >> Pools/Exam-3-12.txt");
		system("cat Pools/Temp-3-09.txt >> Pools/Exam-3-12.txt");
		system("cat Pools/Temp-3-10.txt >> Pools/Exam-3-12.txt");
		system("cat Pools/Temp-3-11.txt >> Pools/Exam-3-12.txt");
		if (!question_3_12("Pools/Temp-3-12.txt")) return 0;
		if (!question_3_13("Pools/Temp-3-13.txt")) return 0;
		if (!question_3_14("Pools/Temp-3-14.txt")) return 0;
		if (!question_3_15("Pools/Temp-3-15.txt")) return 0;
		if (!question_3_16("Pools/Temp-3-16.txt")) return 0;
		system("cat Pools/Temp-3-12.txt > Pools/Exam-3-13.txt");
		system("cat Pools/Temp-3-13.txt >> Pools/Exam-3-13.txt");
		system("cat Pools/Temp-3-14.txt >> Pools/Exam-3-13.txt");
		system("cat Pools/Temp-3-15.txt >> Pools/Exam-3-13.txt");
		system("cat Pools/Temp-3-16.txt >> Pools/Exam-3-13.txt");
		if (!question_3_17("Pools/Temp-3-17.txt")) return 0;
		//if (!question_3_18("Pools/Temp-3-18.txt")) return 0;
		if (!question_3_19("Pools/Temp-3-19.txt")) return 0;
		if (!question_3_20("Pools/Temp-3-20.txt")) return 0;
		if (!question_3_21("Pools/Temp-3-21.txt")) return 0;
		if (!question_3_22("Pools/Temp-3-22.txt")) return 0;
		if (!question_3_23("Pools/Temp-3-23.txt")) return 0;
		if (!question_3_24("Pools/Temp-3-24.txt")) return 0;
		system("cat Pools/Temp-3-17.txt > Pools/Exam-3-14.txt");
		//system("cat Pools/Temp-3-18.txt >> Pools/Exam-3-14.txt");
		system("cat Pools/Temp-3-19.txt >> Pools/Exam-3-14.txt");
		system("cat Pools/Temp-3-20.txt >> Pools/Exam-3-14.txt");
		system("cat Pools/Temp-3-21.txt >> Pools/Exam-3-14.txt");
		system("cat Pools/Temp-3-22.txt >> Pools/Exam-3-14.txt");
		system("cat Pools/Temp-3-23.txt >> Pools/Exam-3-14.txt");
		system("cat Pools/Temp-3-24.txt >> Pools/Exam-3-14.txt");
		if (!question_3_25("Pools/Temp-3-25.txt")) return 0;
		if (!question_3_26("Pools/Temp-3-26.txt")) return 0;
		if (!question_3_27("Pools/Temp-3-27.txt")) return 0;
		if (!question_3_28("Pools/Temp-3-28.txt")) return 0;
		if (!question_3_29("Pools/Temp-3-29.txt")) return 0;
		if (!question_3_30("Pools/Temp-3-30.txt")) return 0;
		system("cat Pools/Temp-3-25.txt > Pools/Exam-3-15.txt");
		system("cat Pools/Temp-3-26.txt >> Pools/Exam-3-15.txt");
		system("cat Pools/Temp-3-27.txt >> Pools/Exam-3-15.txt");
		system("cat Pools/Temp-3-28.txt >> Pools/Exam-3-15.txt");
		system("cat Pools/Temp-3-29.txt >> Pools/Exam-3-15.txt");
		system("cat Pools/Temp-3-30.txt >> Pools/Exam-3-15.txt");

		//if (!question_4_01("Pools/Temp-4-01.txt")) return 0;
		//if (!question_4_02("Pools/Temp-4-02.txt")) return 0;
		//if (!question_4_03("Pools/Temp-4-03.txt")) return 0;
		//if (!question_4_04("Pools/Temp-4-04.txt")) return 0;
		//if (!question_4_05("Pools/Temp-4-05.txt")) return 0;
		//if (!question_4_06("Pools/Temp-4-06.txt")) return 0;
		//system("cat Pools/Temp-4-01.txt > Pools/Exam-3-16.txt");
		//system("cat Pools/Temp-4-02.txt >> Pools/Exam-3-16.txt");
		//system("cat Pools/Temp-4-03.txt >> Pools/Exam-3-16.txt");
		//system("cat Pools/Temp-4-04.txt >> Pools/Exam-3-16.txt");
		//system("cat Pools/Temp-4-05.txt >> Pools/Exam-3-16.txt");
		//system("cat Pools/Temp-4-06.txt >> Pools/Exam-3-16.txt");
		if (!question_4_07("Pools/Temp-4-07.txt")) return 0;
		if (!question_4_08("Pools/Temp-4-08.txt")) return 0;
		if (!question_4_09("Pools/Temp-4-09.txt")) return 0;
		if (!question_4_10("Pools/Temp-4-10.txt")) return 0;
		if (!question_4_11("Pools/Temp-4-11.txt")) return 0;
		if (!question_4_12("Pools/Temp-4-12.txt")) return 0;		
		system("cat Pools/Temp-4-07.txt > Pools/Exam-3-16.txt");
		system("cat Pools/Temp-4-08.txt >> Pools/Exam-3-16.txt");
		system("cat Pools/Temp-4-09.txt >> Pools/Exam-3-16.txt");
		system("cat Pools/Temp-4-10.txt >> Pools/Exam-3-16.txt");
		system("cat Pools/Temp-4-11.txt >> Pools/Exam-3-16.txt");
		system("cat Pools/Temp-4-12.txt >> Pools/Exam-3-16.txt");
		if (!question_4_13("Pools/Temp-4-13.txt")) return 0;
		if (!question_4_14("Pools/Temp-4-14.txt")) return 0;
		if (!question_4_15("Pools/Temp-4-15.txt")) return 0;
		if (!question_4_16("Pools/Temp-4-16.txt")) return 0;
		if (!question_4_17("Pools/Temp-4-17.txt")) return 0;
		//if (!question_4_18("Pools/Temp-4-18.txt")) return 0;
		system("cat Pools/Temp-4-13.txt > Pools/Exam-3-17.txt");
		system("cat Pools/Temp-4-14.txt >> Pools/Exam-3-17.txt");
		system("cat Pools/Temp-4-15.txt >> Pools/Exam-3-17.txt");
		system("cat Pools/Temp-4-16.txt >> Pools/Exam-3-17.txt");
		system("cat Pools/Temp-4-17.txt >> Pools/Exam-3-17.txt");
		//system("cat Pools/Temp-4-18.txt >> Pools/Exam-3-17.txt");
		if (!question_4_19("Pools/Temp-4-19.txt")) return 0;
		if (!question_4_20("Pools/Temp-4-20.txt")) return 0;
		if (!question_4_21("Pools/Temp-4-21.txt")) return 0;
		if (!question_4_22("Pools/Temp-4-22.txt")) return 0;
		if (!question_4_23("Pools/Temp-4-23.txt")) return 0;
		if (!question_4_24("Pools/Temp-4-24.txt")) return 0;
		system("cat Pools/Temp-4-19.txt > Pools/Exam-3-18.txt");
		system("cat Pools/Temp-4-20.txt >> Pools/Exam-3-18.txt");
		system("cat Pools/Temp-4-21.txt >> Pools/Exam-3-18.txt");
		system("cat Pools/Temp-4-22.txt >> Pools/Exam-3-18.txt");
		system("cat Pools/Temp-4-23.txt >> Pools/Exam-3-18.txt");
		system("cat Pools/Temp-4-24.txt >> Pools/Exam-3-18.txt");
		if (!question_4_25("Pools/Temp-4-25.txt")) return 0;
		if (!question_4_26("Pools/Temp-4-26.txt")) return 0;
		if (!question_4_27("Pools/Temp-4-27.txt")) return 0;
		if (!question_4_28("Pools/Temp-4-28.txt")) return 0;
		if (!question_4_29("Pools/Temp-4-29.txt")) return 0;
		if (!question_4_30("Pools/Temp-4-30.txt")) return 0;
		system("cat Pools/Temp-4-25.txt > Pools/Exam-3-19.txt");
		system("cat Pools/Temp-4-26.txt >> Pools/Exam-3-19.txt");
		system("cat Pools/Temp-4-27.txt >> Pools/Exam-3-19.txt");
		system("cat Pools/Temp-4-28.txt >> Pools/Exam-3-19.txt");
		system("cat Pools/Temp-4-29.txt >> Pools/Exam-3-19.txt");
		system("cat Pools/Temp-4-30.txt >> Pools/Exam-3-19.txt");

		if (!question_5_01("Pools/Temp-5-01.txt")) return 0;
		if (!question_5_02("Pools/Temp-5-02.txt")) return 0;
		if (!question_5_03("Pools/Temp-5-03.txt")) return 0;
		if (!question_5_04("Pools/Temp-5-04.txt")) return 0;
		if (!question_5_05("Pools/Temp-5-05.txt")) return 0;
		if (!question_5_06("Pools/Temp-5-06.txt")) return 0;
		if (!question_5_07("Pools/Temp-5-07.txt")) return 0;
		if (!question_5_08("Pools/Temp-5-08.txt")) return 0;
		if (!question_5_09("Pools/Temp-5-09.txt")) return 0;
		if (!question_5_10("Pools/Temp-5-10.txt")) return 0;
		if (!question_5_11("Pools/Temp-5-11.txt")) return 0;
		//if (!question_5_12("Pools/Temp-5-12.txt")) return 0;
		system("cat Pools/Temp-5-01.txt > Pools/Exam-3-20.txt");
		system("cat Pools/Temp-5-02.txt >> Pools/Exam-3-20.txt");
		system("cat Pools/Temp-5-03.txt >> Pools/Exam-3-20.txt");
		system("cat Pools/Temp-5-04.txt >> Pools/Exam-3-20.txt");
		system("cat Pools/Temp-5-05.txt >> Pools/Exam-3-20.txt");
		system("cat Pools/Temp-5-06.txt >> Pools/Exam-3-20.txt");
		system("cat Pools/Temp-5-07.txt >> Pools/Exam-3-20.txt");
		system("cat Pools/Temp-5-08.txt >> Pools/Exam-3-20.txt");
		system("cat Pools/Temp-5-09.txt >> Pools/Exam-3-20.txt");
		system("cat Pools/Temp-5-10.txt >> Pools/Exam-3-20.txt");
		system("cat Pools/Temp-5-11.txt >> Pools/Exam-3-20.txt");
		//system("cat Pools/Temp-5-12.txt >> Pools/Exam-3-20.txt");
		if (!question_5_13("Pools/Temp-5-13.txt")) return 0;
		if (!question_5_14("Pools/Temp-5-14.txt")) return 0;
		if (!question_5_15("Pools/Temp-5-15.txt")) return 0;
		if (!question_5_16("Pools/Temp-5-16.txt")) return 0;
		//if (!question_5_17("Pools/Temp-5-17.txt")) return 0;
		//if (!question_5_18("Pools/Temp-5-18.txt")) return 0;
		system("cat Pools/Temp-5-13.txt > Pools/Exam-3-21.txt");
		system("cat Pools/Temp-5-14.txt >> Pools/Exam-3-21.txt");
		system("cat Pools/Temp-5-15.txt >> Pools/Exam-3-21.txt");
		system("cat Pools/Temp-5-16.txt >> Pools/Exam-3-21.txt");
		//system("cat Pools/Temp-5-17.txt >> Pools/Exam-3-21.txt");
		//system("cat Pools/Temp-5-18.txt >> Pools/Exam-3-21.txt");
		if (!question_5_19("Pools/Temp-5-19.txt")) return 0;
		if (!question_5_20("Pools/Temp-5-20.txt")) return 0;
		if (!question_5_21("Pools/Temp-5-21.txt")) return 0;
		if (!question_5_22("Pools/Temp-5-22.txt")) return 0;
		if (!question_5_23("Pools/Temp-5-23.txt")) return 0;
		system("cat Pools/Temp-5-19.txt > Pools/Exam-3-22.txt");
		system("cat Pools/Temp-5-20.txt >> Pools/Exam-3-22.txt");
		system("cat Pools/Temp-5-21.txt >> Pools/Exam-3-22.txt");
		system("cat Pools/Temp-5-22.txt >> Pools/Exam-3-22.txt");
		system("cat Pools/Temp-5-23.txt >> Pools/Exam-3-22.txt");		
		if (!question_5_24("Pools/Temp-5-24.txt")) return 0;
		if (!question_5_25("Pools/Temp-5-25.txt")) return 0;
		if (!question_5_26("Pools/Temp-5-26.txt")) return 0;
		if (!question_5_27("Pools/Temp-5-27.txt")) return 0;
		//if (!question_5_28("Pools/Temp-5-28.txt")) return 0;
		system("cat Pools/Temp-5-24.txt > Pools/Exam-3-23.txt");
		system("cat Pools/Temp-5-25.txt >> Pools/Exam-3-23.txt");
		system("cat Pools/Temp-5-26.txt >> Pools/Exam-3-23.txt");
		system("cat Pools/Temp-5-27.txt >> Pools/Exam-3-23.txt");
		//system("cat Pools/Temp-5-28.txt >> Pools/Exam-3-23.txt");

		if (!question_6_01("Pools/Temp-6-01.txt")) return 0;
		if (!question_6_02("Pools/Temp-6-02.txt")) return 0;
		if (!question_6_03("Pools/Temp-6-03.txt")) return 0;
		system("cat Pools/Temp-6-01.txt > Pools/Exam-3-24.txt");
		system("cat Pools/Temp-6-02.txt >> Pools/Exam-3-24.txt");
		system("cat Pools/Temp-6-03.txt > Pools/Exam-3-24.txt");
		if (!question_6_04("Pools/Temp-6-04.txt")) return 0;
		if (!question_6_05("Pools/Temp-6-05.txt")) return 0;
		if (!question_6_06("Pools/Temp-6-06.txt")) return 0;
		system("cat Pools/Temp-6-04.txt >> Pools/Exam-3-25.txt");
		system("cat Pools/Temp-6-05.txt > Pools/Exam-3-25.txt");
		system("cat Pools/Temp-6-06.txt >> Pools/Exam-3-25.txt");



		printf("Pools Complete!\n");

		return 1;
	}
	else
	{
		printf("Error, unknown command\n");
		
		return 0;
	}
}









