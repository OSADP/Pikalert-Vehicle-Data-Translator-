Ext.define('emdss.controller.MapAnimation', {
    extend : 'Ext.app.Controller',

    id : 'mapAnimationController',
    config : {
        refs : {
            mapViewDiv : '[itemId=mapViewDiv]',
            mapView : 'main map',
            labelRadarInfo: '#radar-info-label',
            loadUnloadAnimationButton : '#load-unload-animation-button',
            animControlsPanel: '#radar-controls-panel',
            animLoadingPanel: '#radar-loading-panel',
            
            playPauseButton : '[itemId=play-pause-button]',
            fwdButton : '[itemId=step-forward-button]',
            backButton : '[itemId=step-backward-button]'
        },
        control : {
            mapViewDiv : {
                initialize : 'initialize'
            },
            loadUnloadAnimationButton : {
                tap : 'loadUnloadAnimationButtonTapped',
                disabledchange: 'loadUnloadAnimationButtonDisableChanged'
            },
            playPauseButton : {
                tap : 'playPauseButtonTapped'
            },
            fwdButton : {
                tap : 'fwdButtonTapped'
            },
            backButton : {
                tap : 'backButtonTapped'
            }
        }
    },

    /**
     * Initializes this controller with default frame rates.
     * Todo: Add getters and setters for these values when there is a GUI to change them.
     */
    initialize : function() {
        this.frameRateMs = 400;
        this.dwellMs = 1500;
    },
    
    /**
     * Launches this controller -- called after the view components are instantiated
     */
    launch: function() {
        console.log('\n\n\n\n\n\n\n\n\n\nMapAnimation::launch()\n\n\n\n\n\n\n\n\n\n');
        var me = this;
        
        me.alreadyLoadedCounter = 0;

        var animModel = new ral.time.ListTimeSequence(
        {   frameTimes: [new Date(3600000)],
            frameDelayMs: emdss.util.Config.getAnimationFrameDelay(),
            dwellMs: emdss.util.Config.getAnimationDwell()
        });

        var animController = new ral.time.AnimationController(
        {
            timeSequence: animModel,
            usesExternalPlayTimer: true,
            mode: 'FORWARD'
            // mode: 'SWEEP'
        });

        me.animationController = animController;
        me.getLoadUnloadAnimationButton().isLoaded = false;
        me.getPlayPauseButton().isPlaying = false;

        // Wire the sencha buttons to catch animation model updates
        animModel.addListener(me);
    },

    /**
     * Handle play button action by starting/pausing the animation and updating the button icon.
     */
    loadUnloadAnimationButtonTapped : function(btn, e, eOpts) {
        
        console.log("Load/Unload button tapped...");
        
        var playPauseBtn = this.getPlayPauseButton();
        var animControlsPanel = this.getAnimControlsPanel();
        var animLoadingPanel = this.getAnimLoadingPanel();
        
        var cleanupNeeded = false;
        if ( btn.isLoaded ) {
            
            // Update the load button. Do this first, so the playingStateChange shows isLoaded state as false...
            btn.isLoaded = false;
            btn.setIconCls('animation-play-icon');
            
            // Stop the animation, if necessary.
            if ( playPauseBtn.isPlaying ) {
                this.animationController.toggleAnimation();
            }
            
            // Hide the animation controls.
            animControlsPanel.hide();
            animLoadingPanel.hide();
            
            // Set flag that cleanup is needed.
            cleanupNeeded = true;
        }
        else {
            // Update the load button
            btn.isLoaded = true;
            btn.setIconCls('animation-stop-icon');
            this.alreadyLoadedCounter = 0; // Refresh the counter used to hide the "Loading..." label.
            
            // Show the animation controls.
            animControlsPanel.show();
            animLoadingPanel.show();
            
            // Start the animation
            this.animationController.toggleAnimation();
        }
        
        // Clean up the animation -- set latest timestep visible and remove all animation frames from the map.
        if ( cleanupNeeded ) this.cleanUpAfterAnimation();
    },
    
    // MobileMet Version...
    // /**
     // * Handle play button action by starting/pausing the animation and updating the button icon.
     // */
    // loadUnloadAnimationButtonTapped : function(btn, e, eOpts) {
        // if ( btn.isLoaded ) {
            // // Stop the animation
            // btn.isLoaded = false;
            // // btn.setIconCls('animation-play-icon');
            // // this.pauseAnimation();
        // }
        // else {
            // // Start the animation
            // btn.isLoaded = true;
            // // btn.setIconCls('animation-pause-icon');
            // // this.startAnimation();
        // }
// 
        // this.animationController.toggleAnimation();
    // },
    
    /**
     * Handle play/pause button action by simply toggling the animation controller.
     *   The playingStateChanged() callback updates the buttons.
     */
    playPauseButtonTapped : function(btn, e, eOpts) {
        var me = this;
        var map = me.getMapView().getMap();
        
        console.log("MapAnimationController::playPauseButtonTapped()");
        
        me.animationController.toggleAnimation();
        
        var wmsController = emdss.app.getController("WmsData");
    },

    /**
     * Handle fwd button action by pausing the animation, if needed, and stepping forward one frame.
     */
    fwdButtonTapped : function(btn, e, eOpts) {
        var me = this;
        var map = me.getMapView().getMap();
        
        console.log("MapAnimationController::fwdButtonTapped()");

        if (this.isAnimLoggingEnabled()) console.log("Step Forward button tapped...");

        var loadUnloadBtn = this.getLoadUnloadAnimationButton();
        var playPauseBtn = this.getPlayPauseButton();
        if ( playPauseBtn.isPlaying ) {
            playPauseBtn.isPlaying = false;
        }

        this.animationController.stepForward();
        
        var wmsController = emdss.app.getController("WmsData");
    },

    /**
     * Handle back button action by pausing the animation, if needed, and stepping backward one frame.
     */
    backButtonTapped : function(btn, e, eOpts) {
        var me = this;
        var map = me.getMapView().getMap();
        
        console.log("MapAnimationController::fwdButtonTapped()");

        if (this.isAnimLoggingEnabled()) console.log("Step Backward button tapped...");

        var loadUnloadBtn = this.getLoadUnloadAnimationButton();
        var playPauseBtn = this.getPlayPauseButton();
        if ( playPauseBtn.isPlaying ) {
            playPauseBtn.isPlaying = false;
        }

        this.animationController.stepBackward();
        
        var wmsController = emdss.app.getController("WmsData");
    },

    loadUnloadAnimationButtonDisableChanged: function(btn, value, oldValue, eOpts) {
        if (this.isAnimLoggingEnabled()) console.log("loadUnloadAnimationButtonDisableChanged() with value: " + value);
        if ( btn.isLoaded ) {
            btn.setIconCls( value ? 'animation-stop-icon-disabled' : 'animation-stop-icon');
        }
        else {
            btn.setIconCls( value ? 'animation-play-icon-disabled' : 'animation-play-icon');
        }
    },

    fwdButtonDisableChanged: function(btn, value, oldValue, eOpts) {
        if (this.isAnimLoggingEnabled()) console.log("fwdButtonDisableChanged() with value: " + value);
        btn.setIconCls( value ? 'animation-forward-icon-disabled' : 'animation-forward-icon');
    },

    backButtonDisableChanged: function(btn, value, oldValue, eOpts) {
        if (this.isAnimLoggingEnabled()) console.log("backButtonDisableChanged() with value: " + value);
        btn.setIconCls( value ? 'animation-backward-icon-disabled' : 'animation-backward-icon');
    },

    /**
     * Method registered as listener to the ral.time.TimeSequenceModel.firePlayingStateChangedEvent().
     *   Updates the play/pause button to the appropriate look for the current state.
     */
    playingStateChanged: function(isPlaying) {
        
        console.log("MapAnimation::playingStateChanged()");
        
        var loadUnloadBtn = this.getLoadUnloadAnimationButton();
        var playPauseBtn = this.getPlayPauseButton();
        // var timeBtn = emdss.app.getController("Map").getTimeButton();
        if ( isPlaying ) {
           loadUnloadBtn.isLoaded = true;
           playPauseBtn.isPlaying = true;
           playPauseBtn.setIconCls('animation-pause-icon');
        }
        else {
            playPauseBtn.isPlaying = false;
            playPauseBtn.setIconCls('animation-play-icon');
            
            // Sync the visible frame with what is marked as visible.
            this.cleanUpAfterPause();
        }
        
        var wmsController = emdss.app.getController("WmsData");
        if (this.isAnimLoggingEnabled()) wmsController.verifyOneTimestepVisible();
    },

    /**
     * Sync the animation controller with the loaded times.
     *     Called by WmsData controller when:
     *       1) Initial load complete ( in processDelayedLayers() )
     *       2) Data update ( in dataLoaded() callback from LatestWmsMgr )
     */
    syncAnimationTimes: function(requestDate, timesteps) {
        var me = this;
        
        var isPlaying = this.animationController.getIsPlaying();
        var visibleFrame = null;
        
        // Pause the animation, if needed
        if ( isPlaying ) {
            if (me.isAnimLoggingEnabled()) console.log("    Pausing animation...");
            me.animationController.toggleAnimation();
        }

        // Capture the visible frame
        visibleFrame = me.animationController.getTimeSequence().getCurrentFrame();
        if (me.isAnimLoggingEnabled()) console.log("    Visible frame is: " + emdss.util.Util.dateToIsoString(visibleFrame));

        // Set the new times on the ral.times.AnimationController.
        if (me.isAnimLoggingEnabled()) console.log("    Swapping out time sequence frames at: " + emdss.util.Util.dateToIsoString(new Date()));
        me.animationController.getTimeSequence().setTimeSequenceFrames(timesteps);
        
        if (me.isAnimLoggingEnabled()) console.log("    Current frame is now: " + emdss.util.Util.dateToIsoString(this.animationController.getTimeSequence().getCurrentFrame()));

        // Restart the animation, if needed
        if ( isPlaying ) {
            if (me.isAnimLoggingEnabled()) console.log("    Restarting animation...");
            me.animationController.toggleAnimation();
        }
    },

    /**
     * Method registered as listener to the ral.time.TimeSequenceModel.fireFrameChangedEvent().
     *   Updates the visible layer to match the desired frame, based on the active grid
     *   and the passed-in time.
     */
    frameChanged: function(newTime) {
        var me = this;
        
        var wmsController = emdss.app.getController("WmsData");
        var vl = wmsController.getVisibleLayer();

        if ( me.isAnimLoggingEnabled() ) {
            console.log("\n");
            console.log("FRAME CHANGED        FRAME CHANGED        FRAME CHANGED        FRAME CHANGED");
            console.log("                            Old: " + (vl ? vl.emdssDate : 'null'));
            console.log("                            New: " + newTime);
            console.log("FRAME CHANGED        FRAME CHANGED        FRAME CHANGED        FRAME CHANGED");
            console.log("\n");
        }
        
        if ( newTime.getTime() == vl.emdssDate.getTime() ) {
            console.log("Skipping frameChanged() because that timestep is already visible. Name: " + vl.name + " Opacity: " + vl.getOpacity());
            return;
        }
        
        var layers = wmsController.prepareLayersForVisibilitySwitch(newTime);
        var newlyVisibleLayer = layers.new;
        var previouslyVisibleLayer = layers.old;
        
        // Huh? This seems grossly out of place. Wait until the timer executes...
        // wmsController.setVisibleLayer(newlyVisibleLayer);
        
        var animController = me.animationController;
        var completeFrameTransition = me.animationController.completeFrameTransition;
        var tileLoadComplete = function(evt) {
            console.log("");
            console.log("");
            console.log("    XXXXXXXXXXXXXXXXXX LOAD-ONCE NOTICE!!! (" + this.name + ")");
            console.log("");
            console.log("");
            
            var elapsedTime = 98989898;
            if ( me.animationController.getIsPlaying() ) {
                elapsedTime = (new Date().getTime() - me.animationController.getFrameChangeStartTime().getTime());
            }
            var delayTime = me.animationController.getCurrentFrameDelay() - elapsedTime;
            if (me.isAnimLoggingEnabled()) console.log("    Applying completeFrameTransition() from within the loadComplete function. Delay: " + delayTime + "ms. Now: " + emdss.util.Util.dateToIsoStringExact(new Date()) );
            
            var pendingTransitionObj = {'from': previouslyVisibleLayer.name, 'to': newlyVisibleLayer.name, 'continue': true};
            animController.setPendingFrameTransition( pendingTransitionObj );
            setTimeout(function() {
                    // console.log("BEFORE loadComplete() --> completeFrameTransition() call...");
                    completeFrameTransition.apply(animController, [newlyVisibleLayer, previouslyVisibleLayer, newTime, pendingTransitionObj, emdss.util.Config.getGridOverlayOpacity()]);
                    if ( me.isAnimLoggingEnabled() ) {
                        wmsController.printMapRadarLayers();
                        wmsController.verifyOneTimestepVisible();
                    }
                }, Math.max(delayTime, 0));
        };
        
        if ( typeof newlyVisibleLayer.tilesNeedLoading == 'undefined' || newlyVisibleLayer.tilesNeedLoading == true ) {
            google.maps.event.addListenerOnce(newlyVisibleLayer, 'tilesloaded', tileLoadComplete);
        }
        else {
            me.alreadyLoadedCounter++;
            if ( me.alreadyLoadedCounter > emdss.util.Config.getNumRadarTimesteps() ) {
                me.getAnimLoadingPanel().hide();
            }
            
            var elapsedTime = me.animationController.getCurrentFrameDelay();
            if ( me.animationController.getIsPlaying() ) {
                elapsedTime = (new Date().getTime() - me.animationController.getFrameChangeStartTime().getTime());
            }
            var delayTime = me.animationController.getCurrentFrameDelay() - elapsedTime;
            if (me.isAnimLoggingEnabled()) console.log("        Applying completeFrameTransition() directly. Delay: " + delayTime + "ms. Now: " + emdss.util.Util.dateToIsoStringExact(new Date()) );

            var pendingTransitionObj = {'from': previouslyVisibleLayer.name, 'to': newlyVisibleLayer.name, 'continue': true};
            animController.setPendingFrameTransition( pendingTransitionObj );
            setTimeout(function() {
                    // console.log("BEFORE direct completeFrameTransition() call...");
                    completeFrameTransition.apply(animController, [newlyVisibleLayer, previouslyVisibleLayer, newTime, pendingTransitionObj, emdss.util.Config.getGridOverlayOpacity()]);
                    if ( me.isAnimLoggingEnabled() ) {
                        wmsController.printMapRadarLayers();
                        wmsController.verifyOneTimestepVisible();
                    }
                }, Math.max(delayTime, 0));
        }
    },

    /**
     * Method registered as listener to the ral.time.TimeSequenceModel.fireSequenceChangedEvent().
     *   This method gets called after we update the AnimationController with new times.
     * 
     * 1) Initalization of animation times -- easy, since animation won't be loaded.
     * 2) New animation times loaded -- should always be paused, because syncAnimationTimes() pauses before updating
     *      the Animation Controller.
     * 
     *   Make sure this controller and the WmsData controller are still in sync with the animation controller
     *     after the update --> it is possible the visible timestep is no longer a valid timestep.
     */
    sequenceChanged: function(listTimeSequence) {
        var me = this;
        
        var map = me.getMapView().getMap();
        var newTimes = listTimeSequence.getTimeSequenceFrames();
        
        console.log("");
        console.log("");
        console.log("");
        console.log("");
        console.log("");
        console.log("");
        console.log("MapAnimation::sequenceChanged()");
        console.log("");
        console.log("");
        console.log("");
        console.log("");
        console.log("");
        console.log("");
        console.log("");
        
        var wmsController = emdss.app.getController("WmsData");
        if ( me.getLoadUnloadAnimationButton().isLoaded ) {
            // 
            // Animation is loaded. Ensure it is paused, then adjust the visible layer to what's available on the AnimationController.
            // 
            if ( me.getPlayPauseButton().isPlaying ) {
                console.log("\n\n\nERROR: Processing MapAnimation::sequenceChanged() while animation is not paused!!!\n\n");
                return;
            }
            
            var currentFrame = me.animationController.getTimeSequence().getCurrentFrame();
            var visibleLayer = wmsController.getVisibleLayer();
            if ( visibleLayer.emdssDate.getTime() != currentFrame.getTime() ) {
                console.log("\n\nNOTICE: Updating visibleLayer with currentFrame from the AnimationController: " + currentFrame + "\n");
                
                wmsController.ensureTimestepVisibility(currentFrame);
                me.updateRadarStatus(currentFrame);
            }
        }
        else {
            // 
            // Animation not loaded. Make the latest timestep visible.
            // 
            me.makeLatestAnimationTimestepVisible();
            me.updateRadarStatus(wmsController.latestWmsMgr.getLatestDate());
        }
    },

    frameTransitionComplete: function(newTime, newlyVisibleLayer) {
        var me = this;
        var wmsController = emdss.app.getController("WmsData");
        
        if (me.isAnimLoggingEnabled()) console.log("MapAnimation::frameTransitionComplete() to new time: " + newTime);
        
        wmsController.setRadarMap(newlyVisibleLayer);
        wmsController.setVisibleLayer(newlyVisibleLayer);
        
        me.updateRadarStatus(newTime);
        
        if (me.isAnimLoggingEnabled()) wmsController.verifyOneTimestepVisible();
    },
    
    /**
     * Clean up after the pause button is hit:
     *   1) We may have paused during syncAnimationTimes(), so make sure that the visible timestep
     *        is still a valid timestep for the current AnimationController.
     *   2) AnimationController may have cleared the pending frame transition, so make sure that 
     *        the visible layer is actually visible, and others are not.
     */
    cleanUpAfterPause: function() {
        var me = this;
        var wmsController = emdss.app.getController("WmsData");
        
        console.log("MapAnimation::cleanUpAfterPause()");
        
        // 
        // Set the visible layer to the current timestep of the AnimationController.
        // 
        var animationTimeSequence = me.animationController.getTimeSequence();
        var currentAnimationTime = animationTimeSequence.getCurrentFrame();
        
        var map = me.getMapView().getMap();
        var len = map.overlayMapTypes.length;
        wmsController.setRadarMap(null);
        wmsController.setVisibleLayer(null);
        for ( var i = 0; i < len; i++ ) {
            var currLayer = map.overlayMapTypes.getAt(i);
            if ( currLayer == null ) {
                continue;
            }
            
            if (currLayer.emdssDate.getTime() == currentAnimationTime.getTime()) {
                console.log("Found current animation time in MapAnimation::cleanUpAfterPause(): " + currentAnimationTime);
                wmsController.setRadarMap(currLayer);
                wmsController.setVisibleLayer(currLayer);
                break;
            }
        }
        
        var animControllerUpdateRequired = false;
        var vl = wmsController.getVisibleLayer();
        if ( !vl ) {
            // The current animation timestep is no longer a valid timestep. Use the earliest timestep in the existing map layers.
            var earliestTime = null;
            for ( var i = 0; i < len; i++ ) {
                var currLayer = map.overlayMapTypes.getAt(i);
                if ( currLayer == null ) {
                    continue;
                }
                
                if ( earliestTime == null || earliestTime.getTime() > currLayer.emdssDate.getTime() ) {
                    vl = currLayer;
                    earliestTime == currLayer.emdssDate;
                }
            }
            
            animControllerUpdateRequired = true;
            
            // Not necessary -- covered in ensureTimestepVisibility()...
            // wmsController.setRadarMap(vl);
            // wmsController.setVisibleLayer(vl);
        }
        
        // 
        // Make sure the visible layer is the only one visible.
        // 
        wmsController.ensureTimestepVisibility(vl.emdssDate);
        me.updateRadarStatus(vl.emdssDate);
        
        // If the AnimationController had an invalid timestep, update it now.
        if ( animControllerUpdateRequired ) {
            me.animationController.getTimeSequence().setCurrentFrame(vl.emdssDate);
        }
    },
    
    /**
     * Clean up after the stop button (loadUnloadButton) is hit:
     *   Remove all the animation timesteps from the map. Otherwise they continue to load tiles
     *     when the user zooms or pans, reducing responsiveness.
     */
    cleanUpAfterAnimation: function() {
        var me = this;
        var wmsController = emdss.app.getController("WmsData");
        
        me.makeLatestAnimationTimestepVisible();
        
        var latestTimestep = wmsController.latestWmsMgr.getLatestDate();
        me.updateRadarStatus(latestTimestep);
        
        // Set the animation controller to the correct timestep.
        me.animationController.getTimeSequence().setLastFrameCurrent();
        
        var wmsController = emdss.app.getController("WmsData");
        var latestTimestep = wmsController.getVisibleLayer().emdssDate;
        
        // 
        // Remove all the earlier animation layers from the map for performance reasons.
        // 
        var map = me.getMapView().getMap();
        var len = map.overlayMapTypes.length;
        for ( var i = 0; i < len; i++ ) {
            var currLayer = map.overlayMapTypes.getAt(i);
            if ( currLayer == null ) {
                continue;
            }
            
            if (currLayer.emdssDate.getTime() != latestTimestep.getTime()) {
                map.overlayMapTypes.setAt(i, null);
            }
        }
    },
    
    makeLatestAnimationTimestepVisible: function() {
        var me = this;
        var wmsController = emdss.app.getController("WmsData");
        
        console.log("MapAnimation::makeLatestAnimationTimestepVisible()");
        
        // 
        // Set the visible layer to the latest timestep on the AnimationController.
        // 
        var latestTimestep = wmsController.latestWmsMgr.getLatestDate();
        wmsController.ensureTimestepVisibility(latestTimestep);
    },
    
    updateRadarStatus: function(visDate) {
        if ( typeof visDate == 'undefined' || visDate == null ) {
            debugger;
        }
        var timeLabel = this.getLabelRadarInfo();
        timeLabel.setHtml("Radar time: " + emdss.util.Util.dateToPrettyShortDateString(visDate));
    },

    isAnimLoggingEnabled: function() {
        return false;
    }

});
