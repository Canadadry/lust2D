#include "ui_js.h"
#include "regexp.h"
#include "text.h"
#include "ui.h"
#include "../vendor/raylib/raylib.h"
#include "js_helper.h"
#include "splitter.h"
#include "jsx_class.h"
#include "hashmap.h"
#include "raylib_js.h"
#include "vector2.h"
#include "regex.h"
#include <stdio.h>
#include <string.h>

extern HASHMAP(init_node_fn)* hmap_init_node_fn;
extern ARRAY(InitNodeFn1)* array_init_node_fn1;
extern HASHMAP(Texture)* hmap_texture;
extern HASHMAP(Font)* hmap_font;
extern Tree* ui_tree;

UiColor hex_to_rgba(js_State *J,const char *hex) {
    Color color = {0, 0, 0, 255};

    if (hex[0] != '#') {
        js_error(J, "invalid color format");
        return (UiColor){.rgba=color};
    }

    size_t len = strlen(hex);
    if (len == 4) {
        unsigned int r, g, b;
        sscanf(hex, "#%1x%1x%1x", &r, &g, &b);
        color.r = (unsigned char)(r * 17);
        color.g = (unsigned char)(g * 17);
        color.b = (unsigned char)(b * 17);
    } else if (len == 5) {
        unsigned int r, g, b;
        sscanf(hex, "#%2x%2x%2x", &r, &g, &b);
        color.r = (unsigned char)(r);
        color.g = (unsigned char)(g);
        color.b = (unsigned char)(b);
    } else if (len == 7) {
        unsigned int r, g, b;
        sscanf(hex, "#%2x%2x%2x", &r, &g, &b);
        color.r = (unsigned char)(r);
        color.g = (unsigned char)(g);
        color.b = (unsigned char)(b);
    } else if (len == 9) {
        unsigned int r, g, b, a;
        sscanf(hex, "#%2x%2x%2x%2x", &r, &g, &b, &a);
        color.r = (unsigned char)(r);
        color.g = (unsigned char)(g);
        color.b = (unsigned char)(b);
        color.a = (unsigned char)(a);
    } else {
        js_error(J, "invalid color format");
    }

    return (UiColor){.rgba=color};;
}

static UiColor get_color_property(js_State *J, int idx)  {
	if (js_isobject(J, idx) == 0) {
		return (UiColor){.rgba=WHITE};
	};
	if (js_hasproperty(J, idx, "color")){
	    const char* hex_color = get_property_string_or(J,idx,"color","#FFF");
		return hex_to_rgba(J,hex_color);
	}
	return (UiColor){
    	.rgba=(Color){
    		.r = (unsigned char)get_property_number_or(J, idx, "r", 255),
    		.g = (unsigned char)get_property_number_or(J, idx, "g", 255),
    		.b = (unsigned char)get_property_number_or(J, idx, "b", 255),
    		.a = (unsigned char)get_property_number_or(J, idx, "a", 255),
    	},
	};
}

static inline Size get_property_size(js_State *J, int idx,const char* name){
   	if( js_hasproperty(J, idx, name) == 0) {
		return (Size){0};
	}
    int top = js_gettop(J);
    js_getproperty(J, idx, name);
    if(js_isnumber(J,-1)!=0){
        double val = js_tonumber(J,-1);
        js_pop(J, 1);
        return (Size){.kind=SizeKindFixed,.size=(int)val};
    }
    if(js_isobject(J,-1)==0){
        js_pop(J, 1);
        return (Size){0};
    }
	OPTIONAL(string) kind =get_property_string(J,top,"kind");
	if(kind.ok ==false){
	    js_pop(J, 1);
		return (Size){0};
	}
	if(strncmp(kind.value, "fit", 3)==0){
        Size ret= (Size){
            .kind=SizeKindFit,
            .bound={
                .min=get_property_int_or(J,top,"min",0),
                .max=get_property_int_or(J,top,"max",0),
                .pref_use=get_property_string_enum(J,top,"pref","PreferedToNone","PreferedToMin","PreferedToMax"),
            },
        };
        js_pop(J, 1);
        return ret;
	}
	if(strncmp(kind.value, "grow", 3)==0){
        Size ret= (Size){
            .kind=SizeKindGrow,
            .bound={
                .min=get_property_int_or(J,top,"min",0),
                .max=get_property_int_or(J,top,"max",0),
            },
        };
        js_pop(J, 1);
        return ret;
	}
	js_pop(J, 1);
	return (Size){0};
}

static inline Layout get_property_layout(js_State *J, int idx,const char* name){
    OPTIONAL(string) val = get_property_string(J, idx, name);
    if(val.ok==false){
        return LayoutHorizontal;
    }
   	if(strncmp(val.value, "h", 1)==0){
        return LayoutHorizontal;
    }
   	if(strncmp(val.value, "v", 1)==0){
        return LayoutVertical;
    }
   	if(strncmp(val.value, "s", 1)==0){
        return LayoutStack;
    }
    return LayoutHorizontal;
}

static inline Align get_property_align(js_State *J, int idx,const char* name){
    OPTIONAL(string) val = get_property_string(J, idx, name);
    if(val.ok==false){
        return AlignBegin;
    }
   	if(strncmp(val.value, "b", 1)==0){
        return AlignBegin;
    }
   	if(strncmp(val.value, "m", 1)==0){
        return AlignMiddle;
    }
   	if(strncmp(val.value, "e", 1)==0){
        return AlignEnd;
    }
    return AlignBegin;
}

bool walk_children(js_State *J,int idx,int parent){
    if(js_isarray(J,idx)==0){
        return false;
    }
    int len = js_getlength(J,idx);
   	for(int i= 0; i < len; i += 1){
        js_getindex(J, idx,  i);
        if(walk_children(J, js_gettop(J)-1,parent)){
            continue;
        }
        if(js_isnumber(J, -1) == 0){
            js_pop(J,1);
            continue;
        }
        int child= js_tointeger(J, -1);
        js_pop(J,1);
        if(child >= ui_tree->nodes.len||child<0) {
            break;
        }
        link_child(ui_tree,parent,child);
   	}
    return true;
}

static void js_ui_create(js_State *J) {
    const int name=1;
    const int props=2;
    const int children=3;

	int p = get_property_int_or(J,props,"padding",0);
	Node node = (Node){
	    .pos=(VECTOR2(int)){
    		.x=get_property_int_or(J,props,"x",0),
    		.y=get_property_int_or(J,props,"y",0),
		},
		.size=(VECTOR2(Size)){
		    .x=get_property_size(J,props,"w"),
		    .y=get_property_size(J,props,"h"),
		},
		.margin=get_property_int_or(J,props,"margin",0),
		.layout=get_property_layout(J,props,"layout"),
		.padding=(Padding){
           	.left=p,
           	.right=p,
           	.top=p,
           	.bottom=p,
		},
		.align=(VECTOR2(Align)){
    		.x=get_property_align(J, props, "h_align"),
    		.y=get_property_align(J, props, "v_align"),
		},
		.next=-1,
		.first_children=-1,
		.last_children=-1,
		.id={0},
	};
	const char* node_id= get_property_string_or(J,props,"id",NULL);
	if(node_id!=NULL){
	    snprintf(node.id, ID_LEN, "%s",node_id);
	}
	const char* class = get_property_string_or(J,props,"class",NULL);
	if(class!=NULL){
        Splitter splitter = {0};
        init_splitter(&splitter,class, ' ');
        SplitterSlice slice={0};
        while ((slice = next_slice(&splitter)).data != NULL) {
            char classname[128]={0};
            strncpy(classname,slice.data,slice.len);
            init_node_fn* fn = init_node_fn_upsert(hmap_init_node_fn,classname,UpsertActionUpdate);
            if(fn!=NULL && (*fn)!=NULL){
                (*fn)(&node);
            }else{
                for(int i=0;i<array_init_node_fn1->len;i++){
                    if(match(array_init_node_fn1->data[i].regexp,classname)==1){
                        if(array_init_node_fn1->data[i].fn!=NULL){
                            int val=0;
                            sscanf(classname,array_init_node_fn1->data[i].scanf, &val);
                            (*array_init_node_fn1->data[i].fn)(&node,val);
                        }
                        break;
                    }
                }
            }
        }
	}
	if(js_isstring(J, name) != 0){
		const char* title = js_tostring(J, name);
		if(strncmp(title,"rectangle",9)==0){
			node.painter.kind=PAINTER_RECT;
			node.painter.value.rect = (PainterRect){
				.color = get_color_property(J, props),
			};
		}else if(strncmp(title,"item",4)==0){
			node.painter.kind=PAINTER_NONE;
		}else if(strncmp(title,"img",3)==0){
			node.painter.kind=PAINTER_IMG;
			const char* src=get_property_string_or(J,props,"src",NULL);
			if(src!=NULL){
			    strncpy(node.painter.value.img.source, src, SRC_LEN);
			}
			Texture* t = Texture_upsert(hmap_texture,node.painter.value.img.source , UpsertActionCreate);
			if(t!=NULL && !IsTextureValid(*t)){
			    *t = LoadTexture(node.painter.value.img.source);
			}
		}else if(strncmp(title,"npatch",6)==0){
			node.painter.kind=PAINTER_NINE_PATCH;
			node.painter.value.npatch.left=get_property_int_or(J,props,"left",0);
			node.painter.value.npatch.right=get_property_int_or(J,props,"right",0);
			node.painter.value.npatch.top=get_property_int_or(J,props,"top",0);
			node.painter.value.npatch.bottom=get_property_int_or(J,props,"bottom",0);
			const char* src=get_property_string_or(J,props,"src",NULL);
			if(src!=NULL){
			    strncpy(node.painter.value.npatch.source, src, SRC_LEN);
			}
			Texture* t = Texture_upsert(hmap_texture,node.painter.value.npatch.source , UpsertActionCreate);
			if(t!=NULL && !IsTextureValid(*t)){
			    *t = LoadTexture(node.painter.value.npatch.source);
			}
		}else if(strncmp(title,"tile",4)==0){
			node.painter.kind=PAINTER_TILE;
			const char* src=get_property_string_or(J,props,"src",NULL);
			if(src!=NULL){
			    strncpy(node.painter.value.tile.source, src, SRC_LEN);
			}
			Texture* t = Texture_upsert(hmap_texture,node.painter.value.img.source , UpsertActionCreate);
			if(t!=NULL && !IsTextureValid(*t)){
			    *t = LoadTexture(node.painter.value.tile.source);
			}
			node.painter.value.tile.at = (VECTOR2(int)){
    			.x=get_property_int_or(J,props,"at_x",0),
    			.y=get_property_int_or(J,props,"at_y",0),
		    };
			node.painter.value.tile.padding = (VECTOR2(int)){
    			.x=get_property_int_or(J,props,"tile_padding_x",0),
    			.y=get_property_int_or(J,props,"tile_padding_y",0),
		    };
			node.painter.value.tile.size = (VECTOR2(int)){
    			.x=get_property_int_or(J,props,"tile_size_x",0),
    			.y=get_property_int_or(J,props,"tile_size_y",0),
		    };
		}else if(strncmp(title,"txt",4)==0){
			node.painter.kind=PAINTER_TEXT;
			const char* msg=get_property_string_or(J,props,"msg",NULL);
			if(msg!=NULL){
			    strncpy(node.painter.value.text.msg, msg, MSG_LEN);
			}
			node.painter.value.text.color=get_color_property(J, props);
			node.painter.value.text.font_size=get_property_int_or(J,props,"font_size",10);
			node.painter.value.text.spacing=get_property_int_or(J,props,"spacing",node.painter.value.text.font_size/10);
			node.painter.value.text.line_spacing=get_property_int_or(J,props,"line_spacing",node.painter.value.text.font_size/10);
			node.painter.value.text.align=(VECTOR2(Align)){
    		    .x=get_property_align(J, props, "text_h_align"),
    		    .y=get_property_align(J, props, "text_v_align"),
			};
			int is_bmp=1;
			const char* font=get_property_string_or(J,props,"bmp_font",NULL);
			if(font==NULL){
			    is_bmp=0;
				font=get_property_string_or(J,props,"ttf_font",NULL);
			}
			if(font != NULL){
			    strncpy(node.painter.value.text.font, font, SRC_LEN);
			}
			Font* f = Font_upsert(hmap_font,node.painter.value.text.font , UpsertActionCreate);
			if(f!=NULL&&!IsFontValid(*f)){
			    if(is_bmp){
			        *f = LoadFont(node.painter.value.text.font);
				}else{
			        *f = LoadFontEx(
							node.painter.value.text.font,
							node.painter.value.text.font_size,
						 0, 0);
				}
			}
		}else{
			js_error(J, "unknown base ui tag '%s'", title);
			js_pushundefined(J);
			return;
		}
	} else {
		js_error(J, "invalid param type");
		js_pushundefined(J);
		return;
	}

	int parent=ui_tree->nodes.len;
	array_append_Node(&ui_tree->nodes, node);
	walk_children(J,children,parent);
	if(ui_tree->nodes.data[parent].children_count == 0
	&& ui_tree->nodes.data[parent].painter.kind == PAINTER_IMG
	&& node.size.x.kind==SizeKindFit && node.size.x.size==0
	&& node.size.y.kind==SizeKindFit && node.size.y.size==0
	){
	    Texture* t = Texture_upsert(hmap_texture,ui_tree->nodes.data[parent].painter.value.img.source , UpsertActionUpdate);
		if(t != NULL){
		    ui_tree->nodes.data[parent].size.x.kind=SizeKindFixed;
		    ui_tree->nodes.data[parent].size.y.kind=SizeKindFixed;
            ui_tree->nodes.data[parent].size.x.size=t->width;
            ui_tree->nodes.data[parent].size.y.size=t->height;
		}
	}
	js_pushnumber(J, parent);
}

static void js_ui_compute(js_State *J){
	if (ui_tree->nodes.len<=0){
		js_error(J, "empty ui_node, call create before calling draw");
		js_pushundefined(J);
		return;
	}
	if (js_isnumber(J, 1) == 0 ){
		js_error(J, "node index passed to draw should be a number");
		js_pushundefined(J);
		return;
	}
	int idx = js_tointeger(J, 1);
	if(idx >= ui_tree->nodes.len) {
		js_error(J, "node index passed to draw should lesser than ui_node len");
		js_pushundefined(J);
		return;
	}
	Tree* t = ui_tree;
	compute(t, idx);
}

void draw(Tree tree){
	Rectangle rect =(Rectangle){0};
	Texture* t;
	Font f_default = GetFontDefault();
	Font* f=&f_default;
	NPatchInfo np_info={0};
	for(int i=0;i<tree.commands.len;i++){
	    rect.x      = tree.commands.data[i].x;
	    rect.y      = tree.commands.data[i].y;
	    rect.width  = tree.commands.data[i].w;
	    rect.height = tree.commands.data[i].h;
		Painter p =tree.commands.data[i].painter;
	    switch(p.kind){
    	case PAINTER_NONE:
    	    break;
    	case PAINTER_RECT:
    		DrawRectangleRec(rect, p.value.rect.color.rgba);
            break;
        case PAINTER_IMG:
            t = Texture_upsert(hmap_texture,p.value.img.source , UpsertActionUpdate);
            if(t!=NULL){
                DrawTexturePro(*t, (Rectangle){
                    .x=0,.y=0,
                    .width=(float)t->width,
                    .height=(float)t->height,
                },rect, (Vector2){0} , 0, WHITE);
    		}
            break;
        case PAINTER_NINE_PATCH:
            t = Texture_upsert(hmap_texture,p.value.npatch.source , UpsertActionUpdate);
            if(t!=NULL){
                np_info.source=(Rectangle){
                    .x=0,.y=0,
                    .width=(float)t->width,
                    .height=(float)t->height,
                };
                np_info.left = p.value.npatch.left;
                np_info.top = p.value.npatch.top;
                np_info.right = p.value.npatch.right;
                np_info.bottom = p.value.npatch.bottom;
                np_info.layout=NPATCH_NINE_PATCH;
                DrawTextureNPatch(
         			*t,
         			np_info,
         			rect,
                    (Vector2){0} , 0, WHITE
                );
			}
            break;
        case PAINTER_TILE:
            t = Texture_upsert(hmap_texture,p.value.tile.source , UpsertActionUpdate);
    		VECTOR2(int) pos = {
    		    .x=(p.value.tile.padding.x+p.value.tile.size.x)*p.value.tile.at.x+p.value.tile.padding.x,
    			.y=(p.value.tile.padding.y+p.value.tile.size.y)*p.value.tile.at.y+p.value.tile.padding.y
    		};
    		DrawTexturePro(
    			*t,
    			(Rectangle){.x=pos.x, .y=pos.y, .width=p.value.tile.size.x, .height=p.value.tile.size.y},
    			rect,
    			(Vector2){0,0},
    			0,
    			WHITE
    		);
            break;
        case PAINTER_TEXT:
            if(p.value.text.font[0] != 0){
                f = Font_upsert(hmap_font,p.value.text.font , UpsertActionUpdate);
            }
            if(f==NULL){
                f=&f_default;
            }
            if(p.value.text.msg[0] == 0){
                return;
            }
            draw_text(p.value.text.msg,rect,(FontParam){
                .Font=(void*)f,
                .align=p.value.text.align,
                .color=p.value.text.color,
                .line_spacing=p.value.text.line_spacing,
                .spacing=p.value.text.spacing,
                .size=p.value.text.font_size,
            });
		}
	}
}


void dump_ui_command(Tree tree){
	Rectangle rect =(Rectangle){0};
	for(int i=0;i<tree.commands.len;i++){
	    rect.x      = tree.commands.data[i].x;
	    rect.y      = tree.commands.data[i].y;
	    rect.width  = tree.commands.data[i].w;
	    rect.height = tree.commands.data[i].h;
		const char* id = tree.commands.data[i].id;
		printf("command id '%s' %02d : x:%.1f , y:%.1f , w:%.1f , h:%.1f ",id,i,rect.x,rect.y,rect.width,rect.height);
		Painter p =tree.commands.data[i].painter;
	    switch(p.kind){
    	case PAINTER_NONE:
            printf("painter none\n");
    	    break;
    	case PAINTER_RECT:
            printf("painter rect : color r %d g %d b %d a %d\n",p.value.rect.color.rgba.r,p.value.rect.color.rgba.g,p.value.rect.color.rgba.b,p.value.rect.color.rgba.a);
            break;
        case PAINTER_IMG:
            printf("painter img src : '%s'\n",p.value.img.source);
            break;
        case PAINTER_NINE_PATCH:
            printf("painter img npatch : '%s'\n",p.value.npatch.source);
            break;
        case PAINTER_TILE:
            printf("painter img tile : '%s'\n",p.value.tile.source);
            break;
        case PAINTER_TEXT:
            printf("painter txt msg : '%s' color r %d g %d b %d a %d\n",p.value.text.msg,p.value.text.color.rgba.r,p.value.text.color.rgba.g,p.value.text.color.rgba.b,p.value.text.color.rgba.a);
            break;
		}
	}
}

void dump_ui_nodes(Tree tree){
	for(int i=0;i<tree.nodes.len;i++){
		printf("node %2d id '%15s' child %2d next %2d  boxed at : x:%03d , y:%03d , w:%03d , h:%03d ",
           	i,
           	tree.nodes.data[i].id,
           	tree.nodes.data[i].first_children,
           	tree.nodes.data[i].next,
           	tree.nodes.data[i].computed_box.x,
           	tree.nodes.data[i].computed_box.y,
           	tree.nodes.data[i].computed_box.w,
           	tree.nodes.data[i].computed_box.h
		);
		printf("size x kind %5s min %03d max %03d, pref %d ",
           	to_size_kind_name(tree.nodes.data[i].size.x.kind),
           	tree.nodes.data[i].size.x.bound.min,
           	tree.nodes.data[i].size.x.bound.max,
           	tree.nodes.data[i].size.x.bound.pref_use
		);
		printf("size y kind %5s min %03d max %03d, pref %d ",
           	to_size_kind_name(tree.nodes.data[i].size.y.kind),
           	tree.nodes.data[i].size.y.bound.min,
           	tree.nodes.data[i].size.y.bound.max,
           	tree.nodes.data[i].size.y.bound.pref_use
		);
		Painter p = tree.nodes.data[i].painter;
	    switch(p.kind){
    	case PAINTER_NONE:
            printf("painter none\n");
    	    break;
    	case PAINTER_RECT:
            printf("painter rect : color #%2x%2x%2x%2x\n",p.value.rect.color.rgba.r,p.value.rect.color.rgba.g,p.value.rect.color.rgba.b,p.value.rect.color.rgba.a);
            break;
        case PAINTER_IMG:
            printf("painter img src : '%s'\n",p.value.img.source);
            break;
        case PAINTER_NINE_PATCH:
            printf("painter img npatch : '%s'\n",p.value.npatch.source);
            break;
        case PAINTER_TILE:
            printf("painter img tile : '%s'\n",p.value.tile.source);
            break;
        case PAINTER_TEXT:
            printf("painter txt msg : '%s' color #%2x%2x%2x%2x\n",p.value.text.msg,p.value.text.color.rgba.r,p.value.text.color.rgba.g,p.value.text.color.rgba.b,p.value.text.color.rgba.a);
            break;
		}
	}
}


void js_dump_ui_command(js_State* J){
    dump_ui_command(*ui_tree);
}


void js_dump_ui_nodes(js_State* J){
    dump_ui_nodes(*ui_tree);
}

static void js_ui_draw(js_State *J){
	if (ui_tree->commands.len<=0){
		js_error(J, "empty ui_node, call create and compute before calling draw");
		js_pushundefined(J);
		return;
	}
	draw(*ui_tree);
}

bool intersect(PainterCommand *pc, int x, int y){
    if(pc->id[0]==0){
        return false;
    }
    return !(x < pc->x|| y < pc->y || x>(pc->x+pc->w) || y>(pc->y+pc->h)) ;
}

static void js_pick_node(js_State *J){
    if(ui_tree->commands.len <= 0){
        js_pushnull(J);
		return;
    }
    if (js_isnumber(J, 1) == 0 ){
		js_error(J, "first arg passed to ui_pick should be a number");
		js_pushnull(J);
		return;
	}
    if (js_isnumber(J, 2) == 0 ){
		js_error(J, "second arg passed to ui_pick should be a number");
		js_pushnull(J);
		return;
	}
    int x = js_tointeger(J, 1);
    int y = js_tointeger(J, 2);
    for(int i = ui_tree->commands.len-1;i>=0;i--){
        if(intersect(&ui_tree->commands.data[i],x,y)){
            js_pushstring(J, ui_tree->commands.data[i].id);
            return;
        }
    }
    js_pushnull(J);
}

void js_ui_clear(js_State *J){
    ui_tree->nodes.len=0;
    ui_tree->commands.len=0;
    js_pushundefined(J);
}

void bind_ui_func(js_State *J){
    js_newcfunction(J, js_ui_clear, "ui_clear", 0);
	js_setglobal(J, "ui_clear");
    js_newcfunction(J, js_ui_create, "ui_create", 3);
	js_setglobal(J, "ui_create");
	js_newcfunction(J, js_ui_compute, "ui_compute", 1);
	js_setglobal(J, "ui_compute");
	js_newcfunction(J, js_ui_draw, "ui_draw",0);
	js_setglobal(J, "ui_draw");
	js_newcfunction(J, js_pick_node, "ui_pick", 2);
	js_setglobal(J, "ui_pick");
	js_newcfunction(J, js_dump_ui_command, "ui_dump_command", 0);
	js_setglobal(J, "ui_dump_command");
	js_newcfunction(J, js_dump_ui_nodes, "ui_dump_nodes", 0);
	js_setglobal(J, "ui_dump_nodes");


}

VECTOR2(int) mesure_content_fn(void *userdata,Painter p){
    VECTOR2(int) ret = {0};
    Texture* t;
    Font f_default = GetFontDefault();
    Font* f = &f_default;
    switch(p.kind){
    case PAINTER_NONE:
        break;
    case PAINTER_RECT:
        break;
    case PAINTER_NINE_PATCH:
        break;
    case PAINTER_IMG:
        t = Texture_upsert(hmap_texture,p.value.img.source , UpsertActionUpdate);
        if(t!=NULL){
            ret.x = t->width;
            ret.y = t->height;
        }
        break;
    case PAINTER_TILE:
        ret =p.value.tile.size;
        break;
    case PAINTER_TEXT:
        if(p.value.text.font[0] != 0){
            f = Font_upsert(hmap_font,p.value.text.font , UpsertActionUpdate);
        }
        if(f==NULL){
            f=&f_default;
        }
        ret = mesure_text(p.value.text.msg,0,(FontParam){
            .Font=(void*)f,
            .align=(VECTOR2(Align)){.x=AlignBegin,.y=AlignBegin},
            .line_spacing=p.value.text.spacing,
            .spacing=p.value.text.spacing,
            .size=p.value.text.font_size,
        });
        break;
    }
    return ret;
}

int wrap_content_fn(void *userdata,Painter p,int width){
    VECTOR2(int) content = {0};
    Font f_default = GetFontDefault();
    Font* f = &f_default;
    switch(p.kind){
    case PAINTER_NONE:
        break;
    case PAINTER_RECT:
        break;
    case PAINTER_NINE_PATCH:
        break;
    case PAINTER_IMG:
        content = mesure_content_fn(userdata,p);
        return (int)((float)width*(float)content.y/(float)content.x);
    case PAINTER_TILE:
        content = mesure_content_fn(userdata,p);
        return (int)((float)width*(float)content.y/(float)content.x);
    case PAINTER_TEXT:
        if(p.value.text.font[0] != 0){
            f = Font_upsert(hmap_font,p.value.text.font , UpsertActionUpdate);
        }
        if(f==NULL){
            f=&f_default;
        }
        if(p.value.text.msg[0] == 0){
            return 0;
        }
        content = mesure_text(p.value.text.msg,width,(FontParam){
            .Font=(void*)f,
            .align=(VECTOR2(Align)){.x=AlignBegin,.y=AlignBegin},
            .color=p.value.text.color,
            .line_spacing=p.value.text.spacing,
            .spacing=p.value.text.spacing,
            .size=p.value.text.font_size,
        });
        return content.y;
    }
    return 0;
}
