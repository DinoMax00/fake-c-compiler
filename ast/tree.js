$.getJSON("ast.json", function(astJson) {
    var marge = {top:25, bottom:0, left:20, right:0};	
	var svg = d3.select("svg");
	var width = document.getElementById('d3').getBoundingClientRect().height;
	var height = document.getElementById('d3').getBoundingClientRect().width;

    // Create zoom operation
	var g = svg.append("g")
		.attr("transform","translate("+marge.top+","+marge.left+")");
	
    var hierarchyData = d3.hierarchy(astJson);
 
    
    var zoom = d3.zoom()
        .scaleExtent([0.1, 100])
        .on("zoom", () => {
            g.attr(
            "transform",
            "translate(" +
            (d3.event.transform.x + marge.left) +
            "," +
            (d3.event.transform.y + marge.top) +
            ") scale(" +
            d3.event.transform.k +
            ")"
            );
        });
    svg.call(zoom).on("dblclick.zoom", () => {});

    let tmp = hierarchyData.height;
    //创建一个树状图
    var tree = d3.tree()
        .size([200 * tmp ,  200 * (tmp + 1)])
        .separation(function(a,b){
                return (a.parent==b.parent?1:2)/a.depth;
        });

    var treeData = tree(hierarchyData);
    var nodes = treeData.descendants();
    var links = treeData.links();
    var Bézier_curve_generator = d3.linkHorizontal()
    		.x(function(d) { return d.y; })
    		.y(function(d) { return d.x; });

    //绘制边
    g.append("g")
        .selectAll("path")
        .data(links)
        .enter()
        .append("path")
        .attr("d",function(d){
            var start = {x:d.source.x,y:d.source.y};
            var end = {x:d.target.x,y:d.target.y};
            return Bézier_curve_generator({source:start,target:end});
        })
        .attr("fill","none")
        .attr("stroke-width",1);
    
    var gs = g.append("g")
    		.selectAll("g")
    		.data(nodes)
    		.enter()
    		.append("g")
    		.attr("transform",function(d){
    			var cx = d.x;
    			var cy= d.y;
    			return "translate("+cy+","+cx+")";
    		})
            .on("mouseover", d => {
                var transform = d3.event;
                var yPosition = transform.offsetY + 20;
                var xPosition = transform.offsetX + 20;

                var chartTooltip = d3
                  .select(".chartTooltip")
                  .style("left", xPosition + "px")
                  .style("top", yPosition + "px");
                
                chartTooltip.select(".name").text(
                    d.data.value? 
                    "value: " + d.data.value + " , loc: " + d.data.loc
                    : "non-terminal symbol");
        
                chartTooltip.classed("hidden", false);
            })
            .on("mouseout", () => {
                d3.select(".chartTooltip").classed("hidden", true);
            });
    
    //8. 绘制节点和文字
    gs.append("circle")
    .attr("r",6)
    .attr("fill","white")
    .attr("stroke","blue")
    .attr("stroke-width",1)

    //文字
    gs.append("text")
        .attr("x",function(d){
            return -20;//如果某节点有子节点，则对应的文字前移
        })
        .attr("y",-25)
        .attr("dy",10)
        .text(function(d){
            return d.data.name;
        })

});
