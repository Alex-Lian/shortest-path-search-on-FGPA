	component lab8_soc is
		port (
			beginpoint_export      : out   std_logic_vector(31 downto 0);                    -- export
			clk_clk                : in    std_logic                     := 'X';             -- clk
			endpoint_export        : out   std_logic_vector(31 downto 0);                    -- export
			keycode_export         : out   std_logic_vector(7 downto 0);                     -- export
			otg_hpi_address_export : out   std_logic_vector(1 downto 0);                     -- export
			otg_hpi_cs_export      : out   std_logic;                                        -- export
			otg_hpi_data_in_port   : in    std_logic_vector(15 downto 0) := (others => 'X'); -- in_port
			otg_hpi_data_out_port  : out   std_logic_vector(15 downto 0);                    -- out_port
			otg_hpi_r_export       : out   std_logic;                                        -- export
			otg_hpi_reset_export   : out   std_logic;                                        -- export
			otg_hpi_w_export       : out   std_logic;                                        -- export
			path1_export           : out   std_logic_vector(31 downto 0);                    -- export
			path2_export           : out   std_logic_vector(31 downto 0);                    -- export
			pos_x_export           : in    std_logic_vector(9 downto 0)  := (others => 'X'); -- export
			pos_y_export           : in    std_logic_vector(9 downto 0)  := (others => 'X'); -- export
			reset_reset_n          : in    std_logic                     := 'X';             -- reset_n
			sdram_clk_clk          : out   std_logic;                                        -- clk
			sdram_wire_addr        : out   std_logic_vector(12 downto 0);                    -- addr
			sdram_wire_ba          : out   std_logic_vector(1 downto 0);                     -- ba
			sdram_wire_cas_n       : out   std_logic;                                        -- cas_n
			sdram_wire_cke         : out   std_logic;                                        -- cke
			sdram_wire_cs_n        : out   std_logic;                                        -- cs_n
			sdram_wire_dq          : inout std_logic_vector(31 downto 0) := (others => 'X'); -- dq
			sdram_wire_dqm         : out   std_logic_vector(3 downto 0);                     -- dqm
			sdram_wire_ras_n       : out   std_logic;                                        -- ras_n
			sdram_wire_we_n        : out   std_logic;                                        -- we_n
			distancehigh_export    : out   std_logic_vector(31 downto 0);                    -- export
			distancelow_export     : out   std_logic_vector(31 downto 0)                     -- export
		);
	end component lab8_soc;

	u0 : component lab8_soc
		port map (
			beginpoint_export      => CONNECTED_TO_beginpoint_export,      --      beginpoint.export
			clk_clk                => CONNECTED_TO_clk_clk,                --             clk.clk
			endpoint_export        => CONNECTED_TO_endpoint_export,        --        endpoint.export
			keycode_export         => CONNECTED_TO_keycode_export,         --         keycode.export
			otg_hpi_address_export => CONNECTED_TO_otg_hpi_address_export, -- otg_hpi_address.export
			otg_hpi_cs_export      => CONNECTED_TO_otg_hpi_cs_export,      --      otg_hpi_cs.export
			otg_hpi_data_in_port   => CONNECTED_TO_otg_hpi_data_in_port,   --    otg_hpi_data.in_port
			otg_hpi_data_out_port  => CONNECTED_TO_otg_hpi_data_out_port,  --                .out_port
			otg_hpi_r_export       => CONNECTED_TO_otg_hpi_r_export,       --       otg_hpi_r.export
			otg_hpi_reset_export   => CONNECTED_TO_otg_hpi_reset_export,   --   otg_hpi_reset.export
			otg_hpi_w_export       => CONNECTED_TO_otg_hpi_w_export,       --       otg_hpi_w.export
			path1_export           => CONNECTED_TO_path1_export,           --           path1.export
			path2_export           => CONNECTED_TO_path2_export,           --           path2.export
			pos_x_export           => CONNECTED_TO_pos_x_export,           --           pos_x.export
			pos_y_export           => CONNECTED_TO_pos_y_export,           --           pos_y.export
			reset_reset_n          => CONNECTED_TO_reset_reset_n,          --           reset.reset_n
			sdram_clk_clk          => CONNECTED_TO_sdram_clk_clk,          --       sdram_clk.clk
			sdram_wire_addr        => CONNECTED_TO_sdram_wire_addr,        --      sdram_wire.addr
			sdram_wire_ba          => CONNECTED_TO_sdram_wire_ba,          --                .ba
			sdram_wire_cas_n       => CONNECTED_TO_sdram_wire_cas_n,       --                .cas_n
			sdram_wire_cke         => CONNECTED_TO_sdram_wire_cke,         --                .cke
			sdram_wire_cs_n        => CONNECTED_TO_sdram_wire_cs_n,        --                .cs_n
			sdram_wire_dq          => CONNECTED_TO_sdram_wire_dq,          --                .dq
			sdram_wire_dqm         => CONNECTED_TO_sdram_wire_dqm,         --                .dqm
			sdram_wire_ras_n       => CONNECTED_TO_sdram_wire_ras_n,       --                .ras_n
			sdram_wire_we_n        => CONNECTED_TO_sdram_wire_we_n,        --                .we_n
			distancehigh_export    => CONNECTED_TO_distancehigh_export,    --    distancehigh.export
			distancelow_export     => CONNECTED_TO_distancelow_export      --     distancelow.export
		);

