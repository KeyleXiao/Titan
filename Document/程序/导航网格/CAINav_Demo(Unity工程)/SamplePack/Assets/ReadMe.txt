Directory Structure <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

Agents - Some simple agent models.

CAI - The standard CAINav distribution along with some extra general
navigation elements.

Common - Assets shared by all sample scenes.

CrowdDemo - The CrowdManager demo scene.

DemoCommon - General use elements.

FunZone - Models and the base scene for the Fun Zone.

Main - The startup scene and menu for the sample pack.

PathCorridorExplorer - The path corridor explorer scene.

Plugins - The CAINav plugins.

QueryExplorer - The NavmeshQuery explorer scene.

Training Center - Models and base scene for most sample scenes.

The Sample Code <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

The code design for the samples skews toward usefulness for prototyping and 
testing, rather than high efficiency patterns.

If you are looking for sample code that uses the NavmeshQuery and CrowdManager 
for planning, look under the DemoCommon/Nav directory.

NavAgent.cs provides an example of a navigation blackboard with useful utility 
functions.

NavPath.cs provides an example of code to help manage a simple waypoint based 
path.

MoveToCrowd.cs and FollowGoalCrowd.cs provides examples of using the CrowdManger for planning.

MoveToSimple.cs provides an example of using the path corridor for planning.

NavManager.cs provides an example of a simple navigation loop manager.

Miscellany <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

If you really don't like the key layout, check out the /Common/StdButtons.cs 
script.



