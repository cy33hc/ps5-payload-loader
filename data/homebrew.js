async function main() {
    const HOMEBREW_ELF = window.workingDir + '/payload-loader.elf';
    const PAYLOADS_DIR = window.workingDir + '/payloads/';
    const PAYLOAD_TYPES = ['elf', 'bin']

    async function getPayloadList() {
        let listing = await ApiClient.fsListDir(PAYLOADS_DIR);
        return listing.data.filter(entry =>
            PAYLOAD_TYPES.includes(entry.name.slice(-3))).map(entry => {
                const name = entry.name.slice(0, -4);
                return {
                    mainText: name,
                    imgPath: '/fs/' + PAYLOADS_DIR + 'icon0.png',
                    onclick: async () => {
                        return {
                            path: PAYLOADS_DIR + entry.name,
                            daemon: true
                        }
                    }
                };
            });
    }
    return {
        mainText: "Payload Loader 1.2.0",
        secondaryText: 'Payload Loader',
        onclick: async () => {
            let items = await getPayloadList();
            showCarousel(items);
        }
    };
}
