auto SdiViewMediatorTest = [&](){
    rgDataVo.set_bSdi(true);
};

auto SdiViewMediatorTest_Normal = [&]() {
    bSdi = true;
    rgDataVo.set_bSdi( true );
    rgDataVo.set_nSdiSpeedLimit( 99 );
    rgDataVo.set_nSdiType( RgSdiCode::TOLLGATE );
    rgDataVo.set_nSdiDist( 99 );
    rgDataVo.set_nSdiBlockDist( 39 );
    rgDataVo.set_nSdiBlockSpeed( 130 );
    rgDataVo.set_nSdiBlockTime( 47 );
    rgDataVo.set_nSdiBlockAverageSpeed( 85 );
};

auto SdiViewMediatorTest_Alert = [&,this]() {
    bSdi = true;
    if ( mTest == 1 ) {
        rgDataVo.set_nCurSpeed(110);
        rgDataVo.set_bSdi( true );
        rgDataVo.set_nSdiSpeedLimit( 99 );
        rgDataVo.set_nSdiType( RgSdiCode::SPEED_BLOCK_START_POS );
        rgDataVo.set_nSdiDist( 99 );
        rgDataVo.set_nSdiBlockDist( 19 );
        rgDataVo.set_nSdiBlockSpeed( 89 );
        rgDataVo.set_nSdiBlockTime( 19 );
    }
    else {
        rgDataVo.set_nCurSpeed(110);
        rgDataVo.set_bSdi( true );
        rgDataVo.set_nSdiSpeedLimit( 110 );
        rgDataVo.set_nSdiType( RgSdiCode::SPEED_BLOCK_START_POS );
        rgDataVo.set_nSdiDist( 99 );
        rgDataVo.set_nSdiBlockDist( 19 );
        rgDataVo.set_nSdiBlockSpeed( 89 );
        rgDataVo.set_nSdiBlockTime( 19 );
    }
};

auto SdiViewMediatorTest_FixedSectionSpeed = [&]() {
    bSdi = true;
    if ( mTest == 0 ) {
        rgDataVo.set_bSdi( true );
        rgDataVo.set_nSdiSpeedLimit( 99 );
        rgDataVo.set_nSdiType( RgSdiCode::SPEED_BLOCK_START_POS );
        rgDataVo.set_nSdiDist( 99 );
        rgDataVo.set_nSdiBlockDist( 19 );
        rgDataVo.set_nSdiBlockSpeed( 100 );
        rgDataVo.set_nSdiBlockTime( 19 );
        rgDataVo.set_nSdiBlockAverageSpeed( 0 );
        rgDataVo.set_bIsChangeableSpeedType( false );
    }
    else if ( mTest == 1 ) {
        rgDataVo.set_bSdi( true );
        rgDataVo.set_nSdiSpeedLimit( 99 );
        rgDataVo.set_nSdiType( RgSdiCode::SPEED_BLOCK_MID_POS );
        rgDataVo.set_nSdiDist( 99 );
        rgDataVo.set_nSdiBlockDist( 29 );
        rgDataVo.set_nSdiBlockSpeed( 79 );
        rgDataVo.set_nSdiBlockTime( 35 );
        rgDataVo.set_nSdiBlockAverageSpeed( 85 );
        rgDataVo.set_bIsChangeableSpeedType( false );
    }
    else if ( mTest == 2 ) {
        rgDataVo.set_bSdi( true );
        rgDataVo.set_nSdiSpeedLimit( 99 );
        rgDataVo.set_nSdiType( RgSdiCode::SPEED_BLOCK_END_POS );
        rgDataVo.set_nSdiDist( 99 );
        rgDataVo.set_nSdiBlockDist( 39 );
        rgDataVo.set_nSdiBlockSpeed( 130 );
        rgDataVo.set_nSdiBlockTime( 64 );
        rgDataVo.set_nSdiBlockAverageSpeed( 85 );
        rgDataVo.set_bIsChangeableSpeedType( false );
    }
};

auto SdiViewMediatorTest_ChangeableSectionSpeed = [&]() {
    bSdi = true;
    if ( mTest == 0 ) {
        rgDataVo.set_bSdi( true );
        rgDataVo.set_nSdiSpeedLimit( 90 );
        rgDataVo.set_nSdiType( RgSdiCode::SPEED_BLOCK_START_POS );
        rgDataVo.set_nSdiDist( 99 );
        rgDataVo.set_nSdiBlockDist( 19 );
        rgDataVo.set_nSdiBlockSpeed( 100 );
        rgDataVo.set_nSdiBlockTime( 19 );
        rgDataVo.set_nSdiBlockAverageSpeed( 0 );
        rgDataVo.set_bIsChangeableSpeedType( true );
    }
    else if ( mTest == 1 ) {
        rgDataVo.set_bSdi( true );
        rgDataVo.set_nSdiSpeedLimit( 99 );
        rgDataVo.set_nSdiType( RgSdiCode::SPEED_BLOCK_MID_POS );
        rgDataVo.set_nSdiDist( 99 );
        rgDataVo.set_nSdiBlockDist( 29 );
        rgDataVo.set_nSdiBlockSpeed( 79 );
        rgDataVo.set_nSdiBlockTime( 35 );
        rgDataVo.set_nSdiBlockAverageSpeed( 85 );
        rgDataVo.set_bIsChangeableSpeedType( true );
    }
    else if ( mTest == 2 ) {
        rgDataVo.set_bSdi( true );
        rgDataVo.set_nSdiSpeedLimit( 99 );
        rgDataVo.set_nSdiType( RgSdiCode::SPEED_BLOCK_END_POS );
        rgDataVo.set_nSdiDist( 99 );
        rgDataVo.set_nSdiBlockDist( 39 );
        rgDataVo.set_nSdiBlockSpeed( 130 );
        rgDataVo.set_nSdiBlockTime( 47 );
        rgDataVo.set_nSdiBlockAverageSpeed( 85 );
        rgDataVo.set_bIsChangeableSpeedType( true );
    }
};

auto SdiViewMediatorTest_ChangeableSectionSpeedButSpeedLimitNa = [&]() {
    bSdi = true;
    if ( mTest == 0 ) {
        rgDataVo.set_bSdi( true );
        rgDataVo.set_nSdiSpeedLimit( 0 );
        rgDataVo.set_nSdiType( RgSdiCode::SPEED_BLOCK_START_POS );
        rgDataVo.set_nSdiDist( 99 );
        rgDataVo.set_nSdiBlockDist( 19 );
        rgDataVo.set_nSdiBlockSpeed( 100 );
        rgDataVo.set_nSdiBlockTime( 19 );
        rgDataVo.set_nSdiBlockAverageSpeed( 0 );
        rgDataVo.set_bIsChangeableSpeedType( true );
    }
    else if ( mTest == 1 ) {
        rgDataVo.set_bSdi( true );
        rgDataVo.set_nSdiSpeedLimit( 0 );
        rgDataVo.set_nSdiType( RgSdiCode::SPEED_BLOCK_MID_POS );
        rgDataVo.set_nSdiDist( 99 );
        rgDataVo.set_nSdiBlockDist( 29 );
        rgDataVo.set_nSdiBlockSpeed( 79 );
        rgDataVo.set_nSdiBlockTime( 35 );
        rgDataVo.set_nSdiBlockAverageSpeed( 85 );
        rgDataVo.set_bIsChangeableSpeedType( true );
    }
    else if ( mTest == 2 ) {
        rgDataVo.set_bSdi( true );
        rgDataVo.set_nSdiSpeedLimit( 0 );
        rgDataVo.set_nSdiType( RgSdiCode::SPEED_BLOCK_END_POS );
        rgDataVo.set_nSdiDist( 99 );
        rgDataVo.set_nSdiBlockDist( 39 );
        rgDataVo.set_nSdiBlockSpeed( 130 );
        rgDataVo.set_nSdiBlockTime( 47 );
        rgDataVo.set_nSdiBlockAverageSpeed( 85 );
        rgDataVo.set_bIsChangeableSpeedType( true );
    }
};
