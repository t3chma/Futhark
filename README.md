# Futhark
A hobby 2D game engine. Version 2.0
Author: t3chma
--------------------------------------------------
Style Guide:
Code documentation:
/* Breif description.
=TYPE TAG= tag info
A
very
long
description.
^ Link/Reference/See
(parameterName1) Parameter1 has a very
long description
(parameterName2) Parameter2 description
(parameterName3)< Parameter3 is a container intended to be modified. In other words it is an "out" parameter.
< Return description
!Exception1 !Exception2
| A
| code
| example
[Author1, Author2 : mod date]
*/
=BASE= tags classes enchoraged to be used as base classes. info includes what functions are likely to be overriden
=VIRTUAL= tags classes which MUST be inherited from. info includes what functions are to be overriden
=STATIC= tags static classes
=SINGLETON= tags singleton classes
setters don't need to detail the params they set and should have "(Setter)" as thier breif description
getters don't need to detail the return values they get and should have "< Getter" as their breif description

CamelCaseVariables
CAPITAL_CONSTANTS
"Ptr" placed at the end of pointers
"m_" before private variables AND functions
"p_" before protected variables AND functions
Plural forms of variables when in an array, list, exc.
Brackets used like the following:
returnType func(
  parameterName,
  aDifferentParameterName,
  aVeryLongParameterName, aParameterSimilarToThePreviousOne,
) {
  lots
  and
  lots
  of
  code
}
