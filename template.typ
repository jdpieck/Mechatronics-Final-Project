#let project(
  title: "", 
  class: "", 
  authors: "", 
  emails: "", 
  page-height: auto, 
  page-width: 8.5in, 
  accent: rgb("#063e7e"),
  body
) = {
  
  // Set the document's basic properties.
  set document(
    author: authors, 
    title: [#title - #class]
  )
  
  show link: it => underline(text(blue, it))
  set text(font: "IBM Plex Sans", lang: "en")
  
  show heading.where(level: 1): set text(fill: accent)
  show heading.where(level: 2): set text(fill: luma(20%))
  show heading.where(level: 3): set text(fill: luma(25%))

  show heading.where(level: 1): it => context{
    if counter(heading).get() != (0, ) {pagebreak()}
    block(it)
  }

  // show heading.where(level: 2): it => context{
    
  //   if counter(heading).get() != (1, 1) {line(length: 100%, stroke: .25pt)}
  //   v(.2em)
  //   // block(it)
  //   box(it)
  // }
  show figure.caption: set text(.9em, fill: accent)
  show figure.caption.where(body: []): it => it.supplement + [ ] + context it.counter.display()

  set footnote(numbering: "*")
  
  // show bibliography: set heading(numbering: "1)")
  // show bibliography: set par(justify: false)

  show image: it => box(radius: .3em, clip: true, it)

  set grid(column-gutter: 1em, )

  show table: set par(justify: false) 
  show table: set align(left)
  show table.cell.where(y: 0): set text(white, weight: "bold")
  // show table.cell.where(y: 1): set table.cell(fill: blue)
  set table(
    fill: (x,y) =>
      if y == 0 {accent},
    // stroke: frame(none),
    // stroke: (x,y) => (
    //   left: if x == 1 {gray}
    //   )
  )
  show table.cell.where(x: 0): set align(center)    

  // set math.equation(numbering: "(1)")
  show rect: set align(center)
  set par(justify: true)

  set enum(indent: .5em)
  set list(indent: 1em)

  // set raw(lang: "MATLAB")
  // Display inline code 
  show raw.where(block: false): box.with(
    fill: luma(240),
    inset: (x: 3pt, y: 0pt),
    outset: (y: 3pt),
    radius: 2pt,
  )
  
  show raw.where(block: true): set text(.96em)
  // Display block code
  show raw.where(block: true, lang: "cpp"): block.with(
    fill: luma(96%),
    inset: 10pt,
    radius: 4pt,
  )

  show raw.where(lang: "serial", block: true): it => {
    set text(fill: white)
    block(
      fill: rgb("#1F272A"),
      inset: 10pt,
      radius: 4pt,
      width: 100%,
      breakable: false,
      it
    )
  }

  set raw(lang: "c")
  // #set raw(lang: none)
  // show raw.where(block: true): it => figure(it)
  // #set image(width: 75%)
  // #set rect(width: 100%)
  // #show rect: set align(left)

  set page(
    paper: "us-letter", 
    numbering: "1", 
    number-align: right, 
    margin: (
      bottom: .625in,
      rest: .5in
    ), 
    columns: 2,
    height: page-height, 
    width: page-width, 
    footer-descent: 40%,
    footer: context{
      set text(8pt, weight: 300)
      smallcaps[Made with #link("https://typst.app/")[Typst]]
      h(1fr)
      [Jason Daniel Pieck - ]
      text(fill: accent, counter(page).display("1"))
    }
  )
  
  // Title row.
  place(
    top, 
    float: true, 
    scope: "parent",
    {
    grid(
      columns: (auto, 1fr), 
      align: horizon,
      column-gutter: 1em,
      stack(
        spacing: 0.6em, 
        text(weight: 700, 1.75em, fill: accent, title),
        text(weight: 300, 1.5em, class),
      ),
      align(right, 
        stack(
          spacing: 0.6em,
          text(weight: "bold", authors),
          link("mailto:" + emails)[#emails],
          text(weight: 300, .8em, datetime.today().display("[month repr:long] [day], [year]"))
        )
      )  
    )
    
    v(-.3em)
    line(length: 100%, stroke: .5pt)
    v(-9pt)
    line(length: 100%, stroke: .5pt )}
  )
  
  // Main body.

  body
}

///////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////// 


#let problem-num = counter("prob")

#let prob(content, page-break: false) = context {
  problem-num.step()
  
  if problem-num.get().first() > 1 and page-break {pagebreak()}

  let h-content
  if content != [] {h-content = [Problem #context problem-num.display()\. #content]}
  else {h-content = [Problem #context problem-num.display()]  }
  
  heading(level: 1, h-content)
}

#let fig(path) = figure(image(path))

// Rounding
#let r1(num) = calc.round(num, digits: 1)
#let r2(num) = calc.round(num, digits: 2)
#let r3(num) = calc.round(num, digits: 3)
#let r6(num) = calc.round(num, digits: 6)

// Display Functions
#let pythclac(var1, var2) = $sqrt((var1)^2 + (var2)^2)$
#let unitcalc(var1, var2, var3) = $frac((var1)ii + (var2)jj + (var3)kk, sqrt((var1)^2 + (var2)^2 + (var3)^2))$
#let vecdisp(var1, var2, var3) = $(var1) ii + (var2) jj + (var3) kk$
#let det(v1, v2, v3, v4 ,v5, v6) = $mat(ii, jj, kk; v1, v2, v3; v4, v5, v6; delim: "[")$

#let ans(content, alignment: center) = align(alignment)[#block(fill: rgb(255,252,0,255), radius: 4pt, inset: 3pt, content)]
#let ans(equation) = block(fill: rgb(255,252,0,255), radius: 4pt, inset: .5em, {$ equation $})

#let pow(value) = [#math.times 10#super[#value]]
#let powMod(input, order) = {
  let modded = calc.round(input*calc.pow(10, order), digits: 2)
  $modded ee(- order)$
}

// Shorthand
#let then = $quad arrow.double quad$
#let imp = $&=>$
#let there = $therefore quad $
#let also = $quad \& quad$
#let e(var) = $times 10^(var)$
#let ee(var) = $times 10^(var)$
#let eet(var) = [#math.times\10#super[#var]]


// Units
#let milli = $ee(-3)$
#let micro = $ee(-6)$
#let nano = $ee(-9)$
#let kN = $ee(3) "N "$
#let MN = $ee(6) "N "$
#let ft = "ft"
#let Nm = "N⋅m"
#let lbf = "lbf"
#let lbfts = "lb⋅ft/s²"
#let fts = "ft/s²"
#let lb = "lb"
#let npm = "N/m²"
#let kg = "kg"
#let m2 = "m²"
#let new = "N "
#let met = "m "
#let hr = "hr"
#let sec = "sec"
#let ms2 = "m/s²"
#let m3 = "m³"
#let kJ = "kJ"
#let bar = "bar"
#let kW = "kW"
#let degC = "°C"
#let degK= "K "
#let degF= "°F"
#let degR= "°F"
#let net = "net"
#let volt = "V "
#let out = "out"
#let inn = "in"
#let amp = "A "
#let lbfin2 = "lb/in²"
#let ft3lb = "ft³/lb"
#let btulb = "Btu/lb"
#let btulbR = "Btu/lb⋅R"
#let m3kg = "m³/kg"
#let kjkg = "kJ/kg"
#let kPa = "kPa"
#let kjkmol = "kJ/kmol⋅K"
#let kjkgK = "kJ/kg⋅K"
#let mf = $dot(m)$
#let lbs = "lb/s"
#let vel = $arrow(v)$
#let ms = "m/s"
#let kgs = "kg/s"
#let sun = "kJ/kg⋅K"




// // Notation
// #let ii = $hat(i)$
// #let jj = $hat(j)$
// #let kk = $hat(k)$
// #let rr = $hat(r)$
// #let RR = $arrow(R)$
// #let FF = $arrow(F)$
// #let ll(var) = $arrow(lambda)_(var)$
#let xp = $attach(->, tr: plus.circle)$
#let yp = $attach(arrow.t, tr: plus.circle)$
#let rp = $attach(arrow.ccw, tr: plus.circle)$



// #let NV(var) = $arrow(N)_(var)$
// #let NCA = $arrow(N)_(C A)$
// #let NCB = $arrow(N)_(C B)$
// #let (ax, ay, bx, by, cx, cy, dx, dy) = ($F_A_x$, $F_A_y$, $F_B_x$, $F_B_y$, $F_C_x$, $F_C_y$, $F_D_x$, $F_D_y$)
#let (sfx, sfy) = ($Sigma F_x &= 0 $, $Sigma F_y &= 0 $)
#let (sma, smb, smc, smd, sms) = ($Sigma M_A &= 0 $, $Sigma M_B &= 0 $, $Sigma M_C &= 0 $, $Sigma M_D &= 0 $, $Sigma M_"sec" &= 0 = $)


// Constants
#let ge = $9.81 space"m/s"^2 $
  #let cge = 9.81
#let Na = $6.022ee(23) "g/amu"$
  #let cNa = 6.022 // Times 10^23
#let k0 = $9 ee(9) "Vm/C"$
  #let ck0 = 9//*calc.pow(10,9)
#let e0 = $1.6 ee(-19) "C "$
  #let ce0 = 1.6 // Times 10^(-19)
#let rc = $8.134 J\/"mol" dot "K "$
  #let crc =  8.134
#let kel = 273.15
  #let ckel = 273.15

