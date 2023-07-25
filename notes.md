This is a document of the notes about using the torch-version NetShare.

1. Preprocessing has no GPU optimization

2. pre_processed_data and generated_data will take up lots of storage. (e.g. CAIDA, 10 chunk, each takes ~3G)

3. 1 epoch trains the model on all flows (diff from tf-version NetShare where 1 iteration only trains on 1 batch)

   and, with GPU acceleration, 1 epoch on NetShare, 10 chunk, takes ~40s

   and, CAIDA trained 
        80000 iterations w/ 100 batch size in tf version, 
        equiv to 800 epoch in torch verison

4. choice of batch size

    caida: 256, 40s / epoch
    dc: 256 CUDA OOM, 32 is the max batch size that works (takes up 10644MiB / 12288MiB video memory during training). 3min / epoch
        How to optimize gpu usage in dc?
        cpu-only: 
            4096 batch size: ??min / epoch   # ! batch so large that 1 epoch = ?? batch
            2048 batch size: 8min / epoch   # ! batch so large that 1 epoch = 3 batch
            1024 batch size: 11min / epoch  # ! batch so large that 1 epoch = 6 batch
            512 batch size: 14min / epoch                
            256 batch size: 17min / epoch
            128 batch size: 20min / epoch
            64 batch size: 27min / epoch
            32 batch size: 27min / epoch
            16 batch size: 36min / epoch
    
    By decreasing the size of the model, we can train with batch size = 75 at best