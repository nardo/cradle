// CradleTest - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

class CradleTestUserInterface : public UserInterface
{
    test_game *game[2];
    vec2 game_pos[2];
    vec2 game_ext[2];
    uint32 next_port;
public:
    CradleTestUserInterface()
    {
        game[0] = game[1] = 0;
        next_port = 28000;
    }
    ~CradleTestUserInterface()
    {
    }
    
    void onActivate()
    {
        restart_games(true);
    }
    
    void calc_game_frames()
    {
        uint32 frame_half_w = (A.UI.canvasWidth / 2);
        game_pos[0].set(10, 100);
        game_pos[1].set(frame_half_w + 10, 100);
        game_ext[0].set(frame_half_w - 20, frame_half_w - 20);
        game_ext[1].set(frame_half_w - 20, frame_half_w - 20);
    }
    void render()
    {
        glColor3f(1,1,1);
        
        uint32 y = A.UI.canvasHeight - 20;
        
        drawCenteredString(y, 11, "Cradle Test - 'S' restarts server/client, 'C' restarts client/client");
        calc_game_frames();
        
        if(game[0])
            game[0]->render_frame(game_pos[0], game_ext[0]);
        if(game[1])
            game[1]->render_frame(game_pos[1], game_ext[1]);
    }
    
    void idle(uint32 delta)
    {
        _log_prefix() = "Game0";
        if(game[0])
            game[0]->tick(delta);
        _log_prefix() = "Game1";
        if(game[1])
            game[1]->tick(delta);
    }
    
    void onKeyDown(uint32 key)
    {
        if(key == 's')
            restart_games(true);
        else if(key == 'c')
            restart_games(false);
    }
    
    void onKeyUp(uint32 key)
    {
        
    }
    
    void onMouseDown(int32 x, int32 y)
    {
        vec2 mousePos(x,y);
        vec2 window_to_canvas(A.UI.canvasWidth/float32(A.UI.windowWidth), A.UI.canvasHeight/float32(A.UI.windowHeight));
        vec2 canvasPos = mousePos * window_to_canvas;
        calc_game_frames();
        for(uint32 i = 0; i < 2; i++)
        {
            if(!game[i])
                continue;
            vec2 relative_pt((canvasPos.x - game_pos[i].x)/game_ext[i].x, (canvasPos.y - game_pos[i].y)/game_ext[i].y);
            if(relative_pt.x >= 0 && relative_pt.y >= 0 && relative_pt.x <= 1 && relative_pt.y <= 1)
            {
                position new_pos;
                new_pos.x = relative_pt.x;
                new_pos.y = relative_pt.y;
                game[i]->move_my_player_to(new_pos);
            }
        }
    }

    void restart_games(bool game0_is_server)
    {
        if(game[0])
            delete game[0];
        if(game[1])
            delete game[1];
        address interface_bind_address0, interface_bind_address1, ping_address;
        ping_address.set_port(next_port++);
        if(game0_is_server)
            interface_bind_address0 = ping_address;
        else
            interface_bind_address0.set_port(0);
        
        interface_bind_address1.set_port(0);

        game[0] = new test_game(game0_is_server, ping_address);
        game[0]->_net_interface->bind(interface_bind_address0);
        
        game[1] = new test_game(false, ping_address);
        game[1]->_net_interface->bind(interface_bind_address1);
    }
} UITest;
